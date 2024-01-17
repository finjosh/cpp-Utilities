#include "include\Utils\TerminatingFunction.hpp"

std::list<TerminatingFunction::_tFunc> TerminatingFunction::terminatingFunctions;
#ifdef COMMANDPROMPT_H
bool TerminatingFunction::_printToPrompt = false;
#endif

void TerminatingFunction::UpdateFunctions(float deltaTime)
{
    auto function = TerminatingFunction::terminatingFunctions.begin();
    while (function != TerminatingFunction::terminatingFunctions.end())
    {
        function->totalTime += deltaTime;
        data rtnData(deltaTime, function->totalTime);
        function->func.invoke(&rtnData);

        if (rtnData.state == TerminatingFunction::State::Finished)
        {
            auto temp = function;
            function++;
            TerminatingFunction::terminatingFunctions.erase(temp);
            if (TerminatingFunction::terminatingFunctions.size() == 0) return;
            continue;
        }
        function++;
    }

    #ifdef COMMANDPROMPT_H
    if (_printToPrompt)
    {
        printToPromptOnce();
    }
    #endif
}

std::string TerminatingFunction::Add(funcHelper::funcDynamic<data*> function)
{ 
    if (function.valid())
    {
        TerminatingFunction::terminatingFunctions.push_back({function}); 
        
        return function.getTypeid();
    }
    return "";
}

void TerminatingFunction::clear()
{ TerminatingFunction::terminatingFunctions.clear(); }

void TerminatingFunction::remove(const std::string& functionTypeid)
{
    TerminatingFunction::terminatingFunctions.remove_if([functionTypeid](const _tFunc& func){ return functionTypeid == func.func.getTypeid(); });
    return;
}

std::list<std::pair<std::string, std::string>> TerminatingFunction::getStringData()
{
    std::list<std::pair<std::string, std::string>> rtn;

    for (auto func: terminatingFunctions)
    {
        rtn.push_back(func.toString());
    }

    return rtn;
}

#ifdef COMMANDPROMPT_H
void TerminatingFunction::printToPrompt(const bool& print)
{
    _printToPrompt = print;
}

void TerminatingFunction::printToPromptOnce()
{
    Command::Prompt::print("Function Name | Total Time");
    for (std::pair<std::string, std::string> funcData: TerminatingFunction::getStringData())
    {
        Command::Prompt::print(funcData.first + " | " + funcData.second);
    }
}
#endif

#ifdef COMMANDHANDLER_H
void TerminatingFunction::initCommands()
{
    Command::Handler::addCommand({"TFunc", "Any commands for Terminating Functions", {Command::print, "Try using \"help\" fore more info"},
    { {"getData", "Prints the current terminating functions data (printing must be allowed)", {[](){ TerminatingFunction::printToPromptOnce(); }}}
    , {"printData", "[print = false] | Prints the terminating functions data every frame (printing must be allowed)", {[](Command::Data* data)
    {  
        bool temp = TerminatingFunction::printToPrompt;
        if (!Command::isValidInput<bool>("Invalid input", *data, data->getToken(), temp, temp))
            return;
        TerminatingFunction::_printToPrompt = temp;
        data->setReturnStr("Now Printing is ");
        data->addReturnStrLine(temp ? "Enabled" : "Disabled");
        data->setReturnColor(temp ? Command::color(0,255,0) : Command::color(255,0,0));
    }}}
    }});
}
#endif
