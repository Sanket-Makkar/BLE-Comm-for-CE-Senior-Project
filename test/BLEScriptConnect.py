import asyncio
from bleak import BleakClient, BleakScanner

SERVICE_UUID = "12345678-1234-1234-1234-1234567890ab"
CHARACTERISTIC_UUID = "0000FFE1-0000-1000-8000-00805F9B34FB"
DEVICE_NAME = "ESP32-BLE-Sender"

# Stores chunks as they come in
received_chunks = []

# Called whenever the ESP32 sends data
def handle_indication(sender, data):
    text = data.decode(errors="ignore").strip()
    received_chunks.append(text)
    print(f"📥 Received indication: {text}")

async def main():
    print("🔍 Scanning for BLE devices...")
    devices = await BleakScanner.discover()
    print(devices)
    device = next((d for d in devices if d.name and DEVICE_NAME in d.name), None)

    if not device:
        print("❌ Device not found.")
        return

    print(f"✅ Found {device.name} [{device.address}]")

    async with BleakClient(device.address) as client:
        if not client.is_connected:
            print("❌ Failed to connect.")
            return
        print("🔗 Connected.")

        # Subscribe to indications (same API as notifications)
        await client.start_notify(CHARACTERISTIC_UUID, handle_indication)
        print("📡 Subscribed to indications.")

        # Send a request to trigger the chunked message
        await client.write_gatt_char(CHARACTERISTIC_UUID, b"getList")
        print("📤 Sent command: getList")

        # Wait a few seconds to collect incoming chunks
        await asyncio.sleep(3)

        await client.stop_notify(CHARACTERISTIC_UUID)
        print("\n🧾 Final received chunks:")
        for chunk in received_chunks:
            print(f" - {chunk}")

if __name__ == "__main__":
    asyncio.run(main())
