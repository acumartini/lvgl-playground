.PHONY: init check test clean update build upload monitor dmg

VERSION = v0.0.1

init-clion-%:
	platformio init --ide clion --board "${*F}"

init-clion: init-clion-teensy41

init-vscode-%:
	platformio init --ide vscode --board "${*F}"

init-vscode: init-vscode-teensy41

check:
	platformio check

test:
	platformio test

clean:
	platformio run --target clean

update:
	platformio update
	platformio lib update

build:
	platformio run

build-%:
	platformio run -e ${*F}

upload:
	platformio run -t upload

upload-%:
	platformio run -t upload -e ${*F}

monitor:
	screen /dev/cu.usbmodem1432201 115200

dmg: build-release
	rm -rf dist
	mkdir -p dist/dmg
	cp .pio/build/release/firmware.hex dist/dmg
	cp tools/take5-loader dist/dmg
	cp scripts/upload.sh dist/dmg
	hdiutil create -volname take5-${VERSION} -srcfolder dist/dmg -ov -format UDZO dist/Take5.dmg

exe: build-release
	if not exist "dist\exe" mkdir dist\exe
	copy .pio\build\release\firmware.hex dist\exe
	copy tools\take5-loader.exe dist\exe
	copy scripts\upload.bat dist\exe

# osx-installer:
# 	# brew install create-dmg
# 	create-dmg \
# 		--volname "take5 ${VERSION}" \
# 		--background arrangement-audio.png \
# 		--window-pos 200 120 \
# 		--window-size 500 600 \
# 		--icon-size 100 \
# 		--icon take5.app 200 190 \
# 		--hide-extension take5.app \
# 		dist/take5.dmg \
# 		dist/dmg