import asyncio
from bleak import BleakClient, BleakScanner

# Match these with your Arduino sketch
DEVICE_NAME = "XIAO BLE Node"
SERVICE_UUID = "12345678-1234-1234-1234-1234567890ab"
CHARACTERISTIC_UUID = "87654321-4321-4321-4321-ba0987654321"

# Callback for notifications
def handle_notification(sender, data):
    print(f"🔔 Notification from {sender}: {data.decode()}")

async def main():
    print("🔍 Scanning for BLE devices...")
    devices = await BleakScanner.discover()

    xiao_device = None
    for device in devices:
        if device.name and DEVICE_NAME in device.name:
            xiao_device = device
            break

    if not xiao_device:
        print("❌ XIAO BLE Node not found.")
        return

    print(f"✅ Found device: {xiao_device.name} [{xiao_device.address}]")

    async with BleakClient(xiao_device.address) as client:
        print("🔗 Connected to device.")

        # Subscribe to notifications
        await client.start_notify(CHARACTERISTIC_UUID, handle_notification)

        # Write request
        print("📤 Sending: I want data")
        await client.write_gatt_char(CHARACTERISTIC_UUID, b"I want data", response=True)

        # Wait for the device to respond
        print("⏳ Waiting for response...")
        await asyncio.sleep(5)  # You can adjust this based on how long your device takes

        # Done
        await client.stop_notify(CHARACTERISTIC_UUID)
        print("📴 Disconnected.")

if __name__ == "__main__":
    asyncio.run(main())
