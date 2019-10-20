/**
 * @file framework.h
 * @brief
 * @author Nathan Zhou
 * @date 2019-01-17
 * @bug No known bugs.
 */

#ifndef ABC_PLUS_FRAMEWORK_H
#define ABC_PLUS_FRAMEWORK_H

#include <string>
#include <memory>
#include <abc_api.h>
#include <network.h>

namespace abc_plus {
    class Framework {
    public:
        void ReadBlif(const std::string &i_file);

        void ReadBench(const std::string &i_file);

        void WriteBlif(const std::string &o_file);

        NtkPtr GetNtk();

        bool CmdExec(std::string cmd);

        static std::shared_ptr<Framework> GetFramework();

        ~Framework();

        Framework(Framework const &) = delete;

        void operator=(Framework const &) = delete;

    private:
        abc::Abc_Frame_t *abc_frame_;

        Framework();
    };
}

#endif
