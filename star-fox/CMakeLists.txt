cmake_minimum_required(VERSION 3.25.1)
# Configurando o padrão c++
set(CMAKE_CXX_STANDARD 17)

# Configurando o nome e executável do projeto
set(PROJECT_NAME "tp4-star-fox")
project(${PROJECT_NAME})

find_package(SDL2 2.32.2 EXACT REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(${PROJECT_NAME}
        Source/Renderer/Shader.cpp
        Source/Renderer/Shader.h
        Source/Main.cpp
        Source/Math.cpp
        Source/Math.h
        Source/Random.cpp
        Source/Random.h
        Source/Game.cpp
        Source/Game.h
        Source/Actors/Actor.cpp
        Source/Actors/Actor.h
        Source/Components/Component.cpp
        Source/Components/Component.h
        Source/Components/Physics/RigidBodyComponent.cpp
        Source/Components/Physics/RigidBodyComponent.h
        Source/Components/Physics/AABBColliderComponent.cpp
        Source/Components/Physics/AABBColliderComponent.h
        Source/Components/Particles/ParticleSystemComponent.tpp
        Source/Components/Particles/ParticleSystemComponent.h
        Source/Renderer/Renderer.cpp
        Source/Renderer/Renderer.h
        Source/Renderer/VertexArray.h
        Source/Renderer/VertexArray.cpp
        Source/Renderer/Texture.cpp
        Source/Renderer/Texture.h
        Source/Actors/Ship.cpp
        Source/Actors/Ship.h
        Source/Components/Drawing/MeshComponent.cpp
        Source/Components/Drawing/MeshComponent.h
        Source/Renderer/Mesh.cpp
        Source/Renderer/Mesh.h
        Source/Camera.cpp
        Source/Camera.h
        Source/Components/Particles/Bullet.cpp
        Source/Components/Particles/Bullet.h
        Source/AudioSystem.cpp
        Source/AudioSystem.h
        Source/Renderer/Font.h
        Source/Renderer/Font.cpp
        Source/UI/UIButton.cpp
        Source/UI/UIButton.h
        Source/UI/UIElement.cpp
        Source/UI/UIElement.h
        Source/UI/UIImage.cpp
        Source/UI/UIImage.h
        Source/UI/Screens/UIScreen.cpp
        Source/UI/Screens/UIScreen.h
        Source/UI/UIText.cpp
        Source/UI/UIText.h
        Source/UI/Screens/HUD.cpp
        Source/UI/UIRect.cpp
        Source/UI/UIRect.h
        Source/UI/Screens/MainMenu.cpp
        Source/UI/Screens/MainMenu.h
        Source/Actors/Block.cpp
        Source/Actors/Block.h
        Source/UI/Screens/GameOver.cpp
        Source/UI/Screens/GameOver.h
        Source/UI/Screens/PauseMenu.cpp
        Source/UI/Screens/PauseMenu.h
        Source/Components/Particles/Particle.cpp
        Source/Components/Particles/Particle.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE GLEW::GLEW OpenGL::GL SDL2::SDL2 SDL2_image::SDL2_image SDL2_mixer::SDL2_mixer SDL2_ttf::SDL2_ttf)