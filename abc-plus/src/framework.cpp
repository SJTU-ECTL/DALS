/**
 * @file framework.cpp
 * @brief
 * @author Nathan Zhou
 * @date 2019-01-17
 * @bug No known bugs.
 */

#include <string>
#include <framework.h>

namespace abc_plus {
    static Framework *GlobalFrameworkPtr = nullptr;

    void Framework::ReadBlif(const std::string &i_file) {
        CmdExec("read_blif " + i_file);
    }

    void Framework::ReadBench(const std::string &i_file) {
        CmdExec("read_bench " + i_file);
    }

    void Framework::WriteBlif(const std::string &o_file) {
        CmdExec("write_blif " + o_file);
    }

    NtkPtr Framework::GetNtk() {
        NtkPtr ntk = abc::Abc_FrameReadNtk(abc_frame_);
        return ntk;
    }

    bool Framework::CmdExec(std::string cmd) {
        return (bool) Cmd_CommandExecute(abc_frame_, cmd.c_str());
    }

    std::shared_ptr<Framework> Framework::GetFramework() {
        static std::shared_ptr<Framework> framework(new Framework);
        return framework;
    }

    Framework::~Framework() {
        abc::Abc_Stop();
    }

    Framework::Framework() {
        abc::Abc_Start();
        abc_frame_ = abc::Abc_FrameGetGlobalFrame();
    }
}
