/**************************************************************
 *   █████╗ ██╗     ███╗   ███╗   ███╗   ██╗    ██╗██████╗    *
 *  ██╔══██╗██║     ████╗ ████║ ██╔═══██╗████╗  ██║██╔══██╗   *
 *  ███████║██║     ██╔████╔██║ ██║   ██║██╔██╗ ██║██║  ██║   *
 *  ██╔══██║██║     ██║╚██╔╝██║ ██║   ██║██║╚██╗██║██║  ██║   *
 *  ██║  ██║███████╗██║ ╚═╝ ██║ ╚██████╔╝██║ ╚████║██████╔╝   *
 *  ╚═╝  ╚═╝╚══════╝╚═╝     ╚═╝  ╚═════╝ ╚═╝  ╚═══╝╚═════╝    *
 *                                                            *
 *   This file is part of the Almond Project.                 *
 *   AlmondEngine - Modular C++ Game Engine                   *
 *                                                            *
 *   SPDX-License-Identifier: LicenseRef-MIT-NoSell           *
 *                                                            *
 *   Provided "AS IS", without warranty of any kind.          *
 *   Use permitted for non-commercial purposes only           *
 *   without prior commercial licensing agreement.            *
 *                                                            *
 *   Redistribution allowed with this notice.                 *
 *   No obligation to disclose modifications.                 *
 *   See LICENSE file for full terms.                         *
 **************************************************************/
#pragma once
 // anet.hpp — Hybrid transport shim for Almond / Epoch Engine
 //
 // • Default: SteamNetworkingSockets (relay, P2P, NAT punch, QUIC)
 // • Fallback: Asio (UDP) (LAN, dedicated, headless servers)
 //
 // Define ALMOND_WITH_STEAM before including if Steam SDK is available.
 // Link steam_api64.lib (+ tier0.lib, vstdlib.lib) on Windows builds.
 //
 // Build-time deps:
 //   • <asio.hpp> (header-only ≥1.30.2)
 //   • Steam SDK 1.53a (or GameNetworkingSockets ≥v1.4.0)
 //
 // Runtime: call almondnamespace::net::init(); once, almondnamespace::net::poll(); each frame.

#define ASIO_STANDALONE
#define ASIO_HAS_CO_AWAIT
#define _WIN32_WINNT 0x0A00
#include <asio.hpp>

#include <variant>
#include <functional>
#include <span>
#include <string_view>
#include <optional>
#include <memory>
#include <system_error>
#include <cstdint>
#include <cstddef>

#ifdef ALMOND_WITH_STEAM
#  include <steam/steam_api.h>
#  include <steam/steamnetworkingsockets.h>
#endif

namespace almondnamespace::net {

    // Generic views and callback aliases
    using bytes_view = std::span<const std::byte>;
    using send_cb = std::function<void(std::error_code)>;
    using recv_cb = std::function<void(bytes_view)>;

    // UDP backend (Asio)
    struct UdpConn {
        asio::ip::udp::socket sock;
        asio::ip::udp::endpoint peer;

        explicit UdpConn(asio::io_context& io)
            : sock(io) {
        }
    };

#ifdef ALMOND_WITH_STEAM
    // Steam backend
    struct SteamConn {
        HSteamNetConnection h = k_HSteamNetConnection_Invalid;
    };
#endif

    // Hybrid connection handle
    struct Connection {
        std::variant<
            UdpConn
#ifdef ALMOND_WITH_STEAM
            , SteamConn
#endif
        > impl;
    };

    // Backend selector
    enum class backend {
        asio
#ifdef ALMOND_WITH_STEAM
        , steam
#endif
    };

    inline backend g_mode = backend::asio;
    inline std::unique_ptr<asio::io_context> g_io;
#ifdef ALMOND_WITH_STEAM
    inline ISteamNetworkingSockets* g_sn = nullptr;
#endif

    // Initialize: choose Steam if available, else Asio
    inline bool init() {
        // Always create io_context for timers, even if using Steam
        g_io = std::make_unique<asio::io_context>();

#ifdef ALMOND_WITH_STEAM
        if (SteamAPI_Init()) {
            g_mode = backend::steam;
            g_sn = SteamNetworkingSockets();
            return true;
        }
#endif

        // Fallback to Asio
        g_mode = backend::asio;
        return true;
    }

    // Poll: run Asio or Steam callbacks each frame
    inline void poll() {
        if (g_mode == backend::asio) {
            g_io->poll();
#ifdef ALMOND_WITH_STEAM
        }
        else {
            SteamAPI_RunCallbacks();
#endif
        }
    }

    // Dial: "ip:port" for UDP or SteamNetworkingIPAddr string for Steam
    inline Connection dial(std::string_view addr) {
        if (g_mode == backend::asio) {
            auto p = addr.find(':');
            if (p == std::string_view::npos)
                throw std::invalid_argument("Invalid address format: expected ip:port");

            asio::ip::udp::resolver resolver(*g_io);
            asio::ip::udp::resolver::results_type results =
                resolver.resolve(std::string(addr.substr(0, p)), std::string(addr.substr(p + 1)));

            auto ep = *results.begin();

            Connection c{ UdpConn{*g_io} };
            auto& u = std::get<UdpConn>(c.impl);
            u.sock.open(ep.endpoint().protocol());
            u.peer = ep.endpoint();
            return c;
        }
#ifdef ALMOND_WITH_STEAM
        SteamNetworkingIPAddr ipAddr;
        if (!ipAddr.ParseString(addr.data())) {
            throw std::invalid_argument("Invalid SteamNetworkingIPAddr format");
        }
        HSteamNetConnection h = g_sn->ConnectByIPAddress(ipAddr, 0, nullptr);
        return Connection{ SteamConn{h} };
#endif
    }

    // Async send: send data asynchronously on the connection
    inline void async_send(Connection& c, bytes_view data, send_cb cb) {
        std::visit([&](auto& conn) {
            using T = std::decay_t<decltype(conn)>;
            if constexpr (std::is_same_v<T, UdpConn>) {
                conn.sock.async_send_to(
                    asio::buffer(data.data(), data.size()), conn.peer,
                    [cb](std::error_code ec, std::size_t /*bytes_transferred*/) {
                        cb(ec);
                    });
            }
#ifdef ALMOND_WITH_STEAM
            else {
                EResult r = g_sn->SendMessageToConnection(
                    conn.h,
                    data.data(),
                    static_cast<uint32_t>(data.size()),
                    k_nSteamNetworkingSend_Reliable,
                    nullptr);
                cb(r == k_EResultOK
                    ? std::error_code{}
                : std::make_error_code(std::errc::io_error));
            }
#endif
            }, c.impl);
    }

    // async_recv left to caller to wire into their event loop for brevity

} // namespace almondnamespace::net
