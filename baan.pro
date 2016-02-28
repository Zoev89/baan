TEMPLATE = subdirs

SUBDIRS += \
    baanShared\
    baanLib\
    tst \
    baanUI \
    print

baanUI.depends = baanLib
tst.depends = baanLib
baanLib.depends = baanShared
