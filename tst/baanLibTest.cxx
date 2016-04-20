#include "gtest/gtest.h"
#include "AllMocks.h"
#include "baanLib.h"

using ::testing::_;

class baanLibTest : public ::testing::Test {
protected:
    virtual void SetUp()
    {
    }
    virtual void TearDown()
    {
    }
};

TEST_F(baanLibTest, Constructie)
{
    IMessageMock message;
    IMainScreenControlsMock mainScreenControls;
    IMainWindowDrawingMock mainWindowDrawing;
    ITreinenDialoogMock treinenDialoog;
    IBlokInstMock blokInst;
    IWisselDialoogMock wisselDialoog;
    ILampInstDialoogMock lampInstDialoog;
    IKopRichtingDialoogMock kopRichtingDialoog;
    INieuwIODialoogMock nieuwIODialoog;
    IAddBlokDialoogMock addBlok;
    IRegelaarViewUpdatesMock regelaarViewUpdates;
    IRegelaarInstellingenDialoogMock regelaarInstellingenDialoog;
    IThreadSleepMock threadSleep;
    IHardwareComMock hardwareHoog;
    IHardwareComMock hardwareLaag;
    IDraaiSchijfTuningMock draaiSchijfTuning;

    EXPECT_CALL(message,message(_));
    EXPECT_CALL(treinenDialoog, DestroyTreinen());
    EXPECT_CALL(mainScreenControls, mainWindowCloseDeactivate());
    EXPECT_CALL(mainScreenControls, mainWindowTreinDeactivate());
    EXPECT_CALL(mainScreenControls, mainWindowRedraw());
    EXPECT_CALL(mainScreenControls, mainWindowOpenActivate());


    BaanLib baanLibVar(message, mainScreenControls, mainWindowDrawing, treinenDialoog, blokInst, regelaarViewUpdates, regelaarInstellingenDialoog, wisselDialoog, lampInstDialoog, kopRichtingDialoog, nieuwIODialoog,
                       addBlok, threadSleep, hardwareHoog, hardwareLaag, draaiSchijfTuning);
    baanLibVar.BaanClose();
}

TEST_F(baanLibTest, ConstructieNogmaals)
{
    IMessageMock message;
    IMainScreenControlsMock mainScreenControls;
    IMainWindowDrawingMock mainWindowDrawing;
    ITreinenDialoogMock treinenDialoog;
    IBlokInstMock blokInst;
    IWisselDialoogMock wisselDialoog;
    ILampInstDialoogMock lampInstDialoog;
    IKopRichtingDialoogMock kopRichtingDialoog;
    INieuwIODialoogMock nieuwIODialoog;
    IAddBlokDialoogMock addBlok;
    IRegelaarViewUpdatesMock regelaarViewUpdates;
    IRegelaarInstellingenDialoogMock regelaarInstellingenDialoog;
    IThreadSleepMock threadSleep;
    IHardwareComMock hardwareHoog;
    IHardwareComMock hardwareLaag;
    IDraaiSchijfTuningMock draaiSchijfTuning;
    EXPECT_CALL(message,message(_));

    BaanLib baanLibVar(message, mainScreenControls, mainWindowDrawing, treinenDialoog, blokInst, regelaarViewUpdates, regelaarInstellingenDialoog, wisselDialoog, lampInstDialoog, kopRichtingDialoog, nieuwIODialoog,
                       addBlok, threadSleep, hardwareHoog, hardwareLaag, draaiSchijfTuning);
}
