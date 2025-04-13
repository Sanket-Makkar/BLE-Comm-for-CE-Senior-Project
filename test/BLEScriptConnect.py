import asyncio
from bleak import BleakClient, BleakScanner

SERVICE_UUID        = "12345678-1234-1234-1234-1234567890ab"
CHARACTERISTIC_UUID = "0000FFE1-0000-1000-8000-00805F9B34FB"
DEVICE_NAME         = "ESP32-BLE-Sender"

received_chunks = []

def handle_indication(sender, data):
    text = data.decode(errors="ignore").strip()
    received_chunks.append(text)
    print(f"📥 Received indication: {text}")

async def main():
    print("🔍 Scanning for BLE devices...")
    devices = await BleakScanner.discover(timeout=5.0)
    device = next((d for d in devices if d.name == DEVICE_NAME), None)
    if not device:
        print("❌ Device not found.")
        return

    async with BleakClient(device.address) as client:
        # 1) Make sure services/characteristics are discovered
        await client.get_services()
        if not client.is_connected:
            print("❌ Failed to connect.")
            return
        print("🔗 Connected & services discovered.")

        # 2) Subscribe to indications
        await client.start_notify(CHARACTERISTIC_UUID, handle_indication)
        print("📡 Subscribed to indications.")

        # 3) Small pause so the peripheral can settle
        await asyncio.sleep(0.5)

        # 4) Write with response=True if the characteristic expects a confirmation
        cmd = "#Battery;Voltage;#3"
        try:
            await client.write_gatt_char(
                CHARACTERISTIC_UUID,
                cmd.encode(),
                response=True
            )
            print(f"📤 Sent command: {cmd}")
        except Exception as e:
            print(f"❌ Write failed: {e}")
            # we’ll still clean up below

        # 5) Wait for up to N seconds or until you see a “DONE” marker
        for _ in range(30):
            if "DONE" in received_chunks:
                break
            await asyncio.sleep(1)

        # 6) Tear down notifications
        await client.stop_notify(CHARACTERISTIC_UUID)
        print("\n🧾 Final received chunks:")
        for chunk in received_chunks:
            print(f" - {chunk}")

        # 7) Clean disconnect
        if client.is_connected:
            await client.disconnect()
            print("🔌 Disconnected.")

if __name__ == "__main__":
    asyncio.run(main())
