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
        Ref<Texture2D> m_Right;
        Ref<Texture2D> m_Left;
        Ref<Texture2D> m_Top;
        Ref<Texture2D> m_Bottom;
        Ref<Texture2D> m_Front;
        Ref<Texture2D> m_Back;
    };
}