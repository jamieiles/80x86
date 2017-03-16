#include <memory>
#include <string>

#include <gtest/gtest.h>

#include "RTLCPU.h"

double cur_time_stamp = 0;
double sc_time_stamp()
{
    return cur_time_stamp;
}

std::unique_ptr<CPU> get_cpu(const std::string &test_name)
{
    return std::make_unique<RTLCPU<>>(test_name);
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::GTEST_FLAG(filter) += "\
-CmpsRepe/Cmps16Fixture.*:\
CmpsRepe/Cmps8Fixture.*:\
CmpsRepne/Cmps16Fixture.*:\
CmpsRepne/Cmps8Fixture.*:\
Dec/IncMem16Test.*:\
Dec/IncMem8Test.*:\
Dec/IncReg8Test.*:\
Div/DivMem16Test.*:\
Div/DivMem8Test.*:\
Div/DivReg16Test.*:\
Div/DivReg8Test.*:\
EmulateFixture.Aaa:\
EmulateFixture.Aad:\
EmulateFixture.AadSigned:\
EmulateFixture.Aam:\
EmulateFixture.Aam129:\
EmulateFixture.AamDivByZero:\
EmulateFixture.AamSigned:\
EmulateFixture.Aas:\
EmulateFixture.CbwNegative:\
EmulateFixture.CbwPositive:\
EmulateFixture.ClcClearsCarry:\
EmulateFixture.ClcDoesntSetCarry:\
EmulateFixture.CldClearsDirection:\
EmulateFixture.CldDoesntSetDirection:\
EmulateFixture.CliClearsInterrupt:\
EmulateFixture.CliDoesntSetInterrupt:\
EmulateFixture.CmcInverts0:\
EmulateFixture.CmcInverts1:\
EmulateFixture.CmpsbDec:\
EmulateFixture.CmpswDec:\
EmulateFixture.CwdNegative:\
EmulateFixture.CwdPositive:\
EmulateFixture.DSSegmentOverrideIsNop:\
EmulateFixture.Daa:\
EmulateFixture.Das:\
EmulateFixture.DecReg:\
EmulateFixture.EscMem:\
EmulateFixture.EscReg:\
EmulateFixture.Hlt:\
EmulateFixture.InFixed16:\
EmulateFixture.InFixed8:\
EmulateFixture.InVariable16:\
EmulateFixture.InVariable8:\
EmulateFixture.IncFEInvalidReg:\
EmulateFixture.IncReg:\
EmulateFixture.Int3:\
EmulateFixture.IntN:\
EmulateFixture.IntoNotTaken:\
EmulateFixture.IntoTaken:\
EmulateFixture.InvalidRegD0:\
EmulateFixture.InvalidRegD1:\
EmulateFixture.InvalidRegD2:\
EmulateFixture.InvalidRegD3:\
EmulateFixture.InvalidRegFE:\
EmulateFixture.Iret:\
EmulateFixture.JcxzNotTaken:\
EmulateFixture.JcxzTaken:\
EmulateFixture.Lahf:\
EmulateFixture.Lds:\
EmulateFixture.LdsReg:\
EmulateFixture.LeaAxBx:\
EmulateFixture.LeaAxBxSiDisplacement:\
EmulateFixture.Les:\
EmulateFixture.LesReg:\
EmulateFixture.Lock:\
EmulateFixture.Lodsb:\
EmulateFixture.LodsbDec:\
EmulateFixture.Lodsw:\
EmulateFixture.LodswDec:\
EmulateFixture.LoopNotTaken:\
EmulateFixture.LoopTaken:\
EmulateFixture.LoopeNotTaken:\
EmulateFixture.LoopeNotTakenNonZero:\
EmulateFixture.LoopeTaken:\
EmulateFixture.LoopnzNotTaken:\
EmulateFixture.LoopnzNotTakenNonZero:\
EmulateFixture.LoopnzTaken:\
EmulateFixture.MovsbDec:\
EmulateFixture.MovsbInc:\
EmulateFixture.MovswDec:\
EmulateFixture.MovswInc:\
EmulateFixture.Neg16Mem:\
EmulateFixture.Neg16Reg:\
EmulateFixture.Neg8Mem:\
EmulateFixture.Neg8Reg:\
EmulateFixture.Not16Mem:\
EmulateFixture.Not16Reg:\
EmulateFixture.Not8Mem:\
EmulateFixture.Not8Reg:\
EmulateFixture.OutFixed16:\
EmulateFixture.OutFixed8:\
EmulateFixture.OutVariable16:\
EmulateFixture.OutVariable8:\
EmulateFixture.PopMemFF:\
EmulateFixture.PopMemSegmentOverride:\
EmulateFixture.PopReg5X:\
EmulateFixture.PopRegFF:\
EmulateFixture.PopSR:\
EmulateFixture.Popf:\
EmulateFixture.PushMemSegmentOverride:\
EmulateFixture.Pushf:\
EmulateFixture.RetInter:\
EmulateFixture.RetInterAddSp:\
EmulateFixture.RetIntra:\
EmulateFixture.RetIntraAddSp:\
EmulateFixture.Sahf:\
EmulateFixture.ScasbDec:\
EmulateFixture.ScasbDecRepe:\
EmulateFixture.ScasbInc:\
EmulateFixture.ScasbIncRepe:\
EmulateFixture.ScaswDec:\
EmulateFixture.ScaswDecRepe:\
EmulateFixture.ScaswInc:\
EmulateFixture.ScaswIncRepe:\
EmulateFixture.StcDoesntClearCarry:\
EmulateFixture.StcSetsCarry:\
EmulateFixture.StdDoesntClearDirection:\
EmulateFixture.StdSetsDirection:\
EmulateFixture.StiDoesntClearInterrupt:\
EmulateFixture.StiSetsInterrupt:\
EmulateFixture.Stosb:\
EmulateFixture.StosbDec:\
EmulateFixture.Stosw:\
EmulateFixture.StoswDec:\
EmulateFixture.TrapOnInt3:\
EmulateFixture.Wait:\
EmulateFixture.XchgALALNop:\
EmulateFixture.XchgRegAccumulator:\
EmulateFixture.XchgRegMem16:\
EmulateFixture.XchgRegMem8:\
EmulateFixture.XchgRegReg16:\
EmulateFixture.XchgRegReg8:\
EmulateFixture.Xlat:\
IMul/MulMem16Test.*:\
IMul/MulMem8Test.*:\
IMul/MulReg16Test.*:\
IMul/MulReg8Test.*:\
Inc/IncMem16Test.*:\
Inc/IncMem8Test.*:\
Inc/IncReg16Test.*:\
Inc/IncReg8Test.*:\
JmpConditional/JmpFixture.*:\
JmpConditionalAlias/JmpFixture.*:\
Mul/MulMem16Test.*:\
Mul/MulMem8Test.*:\
Mul/MulReg16Test.*:\
Mul/MulReg8Test.*:\
Rclg1/ShiftMem16Test.*:\
Rclg1/ShiftMem8Test.*:\
Rclg1/ShiftReg16Test.*:\
Rclg1/ShiftReg8Test.*:\
RclgCL/ShiftCLTest.*:\
RclgN/ShiftMem16Test.*:\
RclgN/ShiftMem8Test.*:\
RclgN/ShiftReg16Test.*:\
RclgN/ShiftReg8Test.*:\
Rcrg1/ShiftMem16Test.*:\
Rcrg1/ShiftMem8Test.*:\
Rcrg1/ShiftReg16Test.*:\
Rcrg1/ShiftReg8Test.*:\
RcrgCL/ShiftCLTest.*:\
RcrgN/ShiftMem16Test.*:\
RcrgN/ShiftMem8Test.*:\
RcrgN/ShiftReg16Test.*:\
RcrgN/ShiftReg8Test.*:\
Rol1/ShiftMem16Test.*:\
Rol1/ShiftMem8Test.*:\
Rol1/ShiftReg16Test.*:\
Rol1/ShiftReg8Test.*:\
RolCL/ShiftCLTest.*:\
RolN/ShiftMem16Test.*:\
RolN/ShiftMem8Test.*:\
RolN/ShiftReg16Test.*:\
RolN/ShiftReg8Test.*:\
Ror1/ShiftMem16Test.*:\
Ror1/ShiftMem8Test.*:\
Ror1/ShiftReg16Test.*:\
Ror1/ShiftReg8Test.*:\
RorCL/ShiftCLTest.*:\
RorN/ShiftMem16Test.*:\
RorN/ShiftMem8Test.*:\
RorN/ShiftReg16Test.*:\
RorN/ShiftReg8Test.*:\
Sar1/ShiftMem16Test.*:\
Sar1/ShiftMem8Test.*:\
Sar1/ShiftReg16Test.*:\
Sar1/ShiftReg8Test.*:\
SarCL/ShiftCLTest.*:\
SarN/ShiftMem16Test.*:\
SarN/ShiftMem8Test.*:\
SarN/ShiftReg16Test.*:\
SarN/ShiftReg8Test.*:\
SegmentOverride/MovOverride.*:\
Shl1/ShiftMem16Test.*:\
Shl1/ShiftMem8Test.*:\
Shl1/ShiftReg16Test.*:\
Shl1/ShiftReg8Test.*:\
ShlCL/ShiftCLTest.*:\
ShlN/ShiftMem16Test.*:\
ShlN/ShiftMem8Test.*:\
ShlN/ShiftReg16Test.*:\
ShlN/ShiftReg8Test.*:\
Shr1/ShiftMem16Test.*:\
Shr1/ShiftMem8Test.*:\
Shr1/ShiftReg16Test.*:\
Shr1/ShiftReg8Test.*:\
ShrCL/ShiftCLTest.*:\
ShrN/ShiftMem16Test.*:\
ShrN/ShiftMem8Test.*:\
ShrN/ShiftReg16Test.*:\
ShrN/ShiftReg8Test.*";

    return RUN_ALL_TESTS();
}
