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

    EXPECT_CALL(message,message(_));

    BaanLib baanLibVar(message, mainScreenControls, mainWindowDrawing, treinenDialoog, blokInst, regelaarViewUpdates, regelaarInstellingenDialoog, wisselDialoog, lampInstDialoog, kopRichtingDialoog, nieuwIODialoog,
                       addBlok, threadSleep);
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

    EXPECT_CALL(message,message(_));

    BaanLib baanLibVar(message, mainScreenControls, mainWindowDrawing, treinenDialoog, blokInst, regelaarViewUpdates, regelaarInstellingenDialoog, wisselDialoog, lampInstDialoog, kopRichtingDialoog, nieuwIODialoog,
                       addBlok, threadSleep);
}
