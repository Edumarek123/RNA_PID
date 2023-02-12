BOARD? = esp32:nodemcu-32s
PORT? = /dev/ttyUSB0
BUILD = build

.PHONY: default lint all flash clean

default: lint all flash clean

lint:
	cpplint --extentions = ino --filter = -legal/copyright *.ino

all:
	arduino-cli compile --fqbn $(BOARD) --output-dir $(BUILD) ./

flash:
	arduino-cli upload --fqbn $(BOARD) --port $(PORT) --input-dir $(BUILD)

clean:
	rm -r build
