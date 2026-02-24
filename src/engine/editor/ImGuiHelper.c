#include "ImGuiHelper.h"

#include "imgui.h"
#include "../utils/Cvars.h"

bool ImGuiHelper_CvarCheckbox(const char* label, const char* cvar)
{
    bool value = Cvars_GetAsBool(cvar);
    if (ImGui::Checkbox(label, &value))
    {
        Cvars_SetAsBool(cvar, value);
        return true;
    }
    return false;
}
