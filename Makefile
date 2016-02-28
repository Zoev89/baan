
all: 
	$(MAKE) -j2 -C src

run:
	cd ../build-baan-Desktop-Debug/baanUI;./baanUI ../../ezb/baan.blk

edit:
	cd ./bin;./baan -editMode $(BAAN_PATH)/baan.blk

clean:
	make -C src clean

indent:
	make -C src indent
	
backup: clean
	cd tst; make clean; cd ..
	cp -r * /media/eric/ERIC128/linuxBaan/baan
	cp -r $(BAAN_PATH)/* /media/eric/ERIC128/linuxBaan/ezb
    
restore:
	cp -r /media/eric/ERIC128/linuxBaan/baan/src/* src
	cp -r /media/eric/ERIC128/linuxBaan/baan/shared/* shared
	cp -r /media/eric/ERIC128/linuxBaan/ezb/* $(BAAN_PATH)

tar: clean
	date --rfc-3339='date'>VERSION.txt;cd ..;tar -czf baan.tar.gz baan
    
applyTar:
	cd ..;cp -r baan baanOld; rm -r baan; tar -xzf baan.tar.gz


