#ifndef MKTRCMDFILLER_H
#define MKTRCMDFILLER_H

#include <string>
#include <list>
class MktrCmdFiller
{
protected:
    std::string cmdLine;
    static std::string replace(const std::string& str, const std::string& from, const std::string& to);

public:
    MktrCmdFiller(const std::string& cmd);
    virtual std::list<std::string> combinations();
};

#endif
