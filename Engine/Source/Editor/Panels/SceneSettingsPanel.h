#pragma once 

#include "Runtime/Renderer/Texture.h"

namespace Flame
{
    class SceneSettingsPanel
    {
    public:
        SceneSettingsPanel();

        void OnImGuiRender(bool* pOpen);

    private:
        std::vector<std::string> m_Paths;

        //Visualization
        Ref<Texture> m_Right;
        Ref<Texture> m_Left;
        Ref<Texture> m_Top;
        Ref<Texture> m_Bottom;
        Ref<Texture> m_Front;
        Ref<Texture> m_Back;
    };
}