import asyncio
from bleak import BleakClient, BleakScanner

SERVICE_UUID        = "12345678-1234-1234-1234-1234567890ab"
CHARACTERISTIC_UUID = "0000FFE1-0000-1000-8000-00805F9B34FB"
DEVICE_NAME         = "ESP32-BLE-Sender"

async def main():
    print("Scanning for BLE devices...")
    devices = await BleakScanner.discover(timeout=5.0)
    device = next((d for d in devices if d.name == DEVICE_NAME), None)
    if not device:
        print("Device not found.")
        return

    print(f"Found {device.name} [{device.address}]")

    received_chunks = []

    def handle_indication(sender, data):
        text = data.decode(errors="ignore").strip()
        received_chunks.append(text)
        print(f"Received indication: {text}")

    client = BleakClient(device.address)
    try:
        await client.connect()
        if not client.is_connected:
            print("Failed to connect.")
            return
        print("Connected.")

        _ = client.services
        print("Services discovered.")

        await client.start_notify(CHARACTERISTIC_UUID, handle_indication)
        print("Subscribed to indications.")

        await asyncio.sleep(0.5)

        received_chunks.clear()

        cmd = "#Battery;Voltage;150"
        try:
            await client.write_gatt_char(CHARACTERISTIC_UUID, cmd.encode(), response=True)
            print(f"Sent command: {cmd}")
        except Exception as e:
            print(f"Write failed: {e}")
            return

        await asyncio.sleep(60)

        await client.stop_notify(CHARACTERISTIC_UUID)
        print("Unsubscribed from indications.")

        print("\nFinal received chunks:")
        if not received_chunks:
            print("(none received)")
        else:
            for chunk in received_chunks:
                print(f"{chunk}")

    except Exception as e:
        print(f"Error: {e}")
    finally:
        if client.is_connected:
            await client.disconnect()
            print("Disconnected.")

if __name__ == "__main__":
    asyncio.run(main())
