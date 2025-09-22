 
#!/bin/bash

# Get the script's directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Run the Vulkan application from the script directory
#"$SCRIPT_DIR/VulkanApplication1"
"$SCRIPT_DIR/Bin/linux-debug/cmakeapp1/cmakeapp1"
