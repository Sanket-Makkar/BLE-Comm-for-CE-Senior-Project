import asyncio
from bleak import BleakClient, BleakScanner

# Full 128‑bit UUIDs (recommended)
SERVICE_UUID = "12345678-1234-1234-1234-1234567890ab"
CHARACTERISTIC_UUID = "87654321-4321-4321-4321-ba0987654321"

# The deviceName you passed into NimBLEDevice::init(...)
DEVICE_NAME = "ESP32-BLE-Sender"

# This function handles incoming notifications from the BLE device
def handle_notification(sender, data):
    text = data.decode(errors="ignore")
    print(f"🔔 Notification from {sender}: {text}")

async def main():
    print("🔍 Scanning for BLE devices...")

    # Discover nearby BLE devices
    devices = await BleakScanner.discover()

    # Find the device by its name (you can customize this to match the device's name)
    device = next((d for d in devices if d.name and DEVICE_NAME in d.name), None)

    if not device:
        print("❌ Device not found.")
        return

    print(f"✅ Found {device.name} [{device.address}]")

    # Connect to the discovered device
    async with BleakClient(device.address) as client:
        if not client.is_connected:
            print("❌ Failed to connect.")
            return
        print("🔗 Connected.")

        # Subscribe to notifications for the characteristic
        print(f"🔔 Subscribing to notifications on {CHARACTERISTIC_UUID}...")
        await client.start_notify(CHARACTERISTIC_UUID, handle_notification)

        # Optionally, read the current value of the characteristic once
        try:
            value = await client.read_gatt_char(CHARACTERISTIC_UUID)
            print(f"📦 Current value of characteristic: {value.decode('utf-8')}")
        except Exception as e:
            print(f"❌ Failed to read characteristic: {e}")

        # Keep the connection open and listen for notifications
        try:
            print("⏳ Waiting for notifications (Ctrl+C to stop)...")
            while True:
                await asyncio.sleep(1.0)
        except KeyboardInterrupt:
            print("\n✋ Stopping notifications...")

        # Stop notifications before disconnecting
        await client.stop_notify(CHARACTERISTIC_UUID)
        print("📴 Disconnected.")

if __name__ == "__main__":
    asyncio.run(main())
