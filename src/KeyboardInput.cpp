#include "KeyboardInput.h"

#include <iostream>
#include <limits>

namespace lve
{
    void KeyboardInput::KeyEvent(int key, int scancode, int action, int mods)
    {
        m_MoveDir = glm::vec3(0.f);

        const glm::vec3 forwardDir{ sin(m_Yaw), sin(-m_Pitch)*cos(m_Yaw), cos(m_Yaw) };
        const glm::vec3 rightDir{ forwardDir.z, 0.f, -forwardDir.x };
        const glm::vec3 upDir{ 0.f, -1.f, 0.f };

        // wasd movement of camera
        if (key == GLFW_KEY_W && (action == GLFW_REPEAT || action == GLFW_PRESS))
        {
            m_MoveDir += forwardDir;
        }
        if (key == GLFW_KEY_S && (action == GLFW_REPEAT || action == GLFW_PRESS))
        {
            m_MoveDir += -forwardDir ;
        }
        if (key == GLFW_KEY_A && (action == GLFW_REPEAT || action == GLFW_PRESS))
        {
            m_MoveDir += -rightDir;
        }
        if (key == GLFW_KEY_D && (action == GLFW_REPEAT || action == GLFW_PRESS))
        {
            m_MoveDir += rightDir;
        }
        if(key == GLFW_KEY_E && action == GLFW_RELEASE)
        {
            m_bRandomizeTerrain = true;
        }
    }

    void KeyboardInput::MouseMove(GLFWwindow* window, double xpos, double ypos)
    {

        int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
        if (state == GLFW_PRESS)
        {
            float dx = static_cast<float>(xpos) - m_DragStart.x;
            float dy = static_cast<float>(ypos) - m_DragStart.y;
            if (dx > 0)
            {
                m_Rotation.y += -dx/ 10000;
            }
            else if (dx < 0)
            {
                m_Rotation.y += -dx/ 10000;
            }
            if (dy > 0)
            {
                m_Rotation.x += dy/ 10000;
            }
            else if (dy < 0)
            {
                m_Rotation.x += dy/ 10000;
            }
        }
    }

    void KeyboardInput::MouseEvent(GLFWwindow* window, int button, int action, int mods)
    {
        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        {
            std::cout << "right mouse button pressed\n";
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            m_DragStart.x = static_cast<float>(xpos);
            m_DragStart.y = static_cast<float>(ypos);
        }
    }

    void KeyboardInput::Update(GameObject& gameObject, float deltaTime)
    {
    	gameObject.transform.rotation = m_Rotation;
        m_Yaw = gameObject.transform.rotation.y;
        m_Pitch = gameObject.transform.rotation.x;

		gameObject.transform.translation += m_MoveSpeed * deltaTime * m_MoveDir;
    }
}
