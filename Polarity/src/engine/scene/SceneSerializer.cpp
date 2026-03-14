#include "polpch.h"
#include "SceneSerializer.h"

#include "Entity.h"
#include "Components.h"

#include "engine/Project/Project.h"

#include <yaml-cpp/yaml.h>
#include <fstream>

namespace YAML {

    template<>
    struct convert<glm::vec2>
    {
        static Node encode(const glm::vec2& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, glm::vec2& rhs)
        {
            if (!node.IsSequence() || node.size() != 2)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            return true;
        }
    };

    template<>
    struct convert<glm::vec3>
    {
        static Node encode(const glm::vec3& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, glm::vec3& rhs)
        {
            if (!node.IsSequence() || node.size() != 3)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            return true;
        }
    };

    template<>
    struct convert<glm::vec4>
    {
        static Node encode(const glm::vec4& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.push_back(rhs.w);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, glm::vec4& rhs)
        {
            if (!node.IsSequence() || node.size() != 4)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            rhs.w = node[3].as<float>();
            return true;
        }
    };
}
namespace Polarity
{
    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
        return out;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
        return out;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
        return out;
    }

    SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
        : m_Scene(scene)
    {
        
    }

    static void SerializeEntity(YAML::Emitter& out, Entity entity)
    {
        out << YAML::BeginMap;  // Entity
        out << YAML::Key << "Entity" << YAML::Value << entity.GetID();

        if (entity.HasComponent<NameComponent>())
        {
            out << YAML::Key << "NameComponent";
            out << YAML::BeginMap;  // Component
            
            auto& name = entity.GetComponent<NameComponent>().Name;
            out << YAML::Key << "Name" << YAML::Value << name;

            out << YAML::EndMap;    // Component
        }
        if (entity.HasComponent<TransformComponent>())
        {
            out << YAML::Key << "TransformComponent";
            out << YAML::BeginMap;  // Component

            auto& transform = entity.GetComponent<TransformComponent>();
            out << YAML::Key << "Position" << YAML::Value << transform.Position;
            out << YAML::Key << "Rotation" << YAML::Value << transform.Rotation;
            out << YAML::Key << "Scale"    << YAML::Value << transform.Scale;

            out << YAML::EndMap;    // Component
        }
        if (entity.HasComponent<CameraComponent>())
        {
            out << YAML::Key << "CameraComponent";
            out << YAML::BeginMap;  // Component

            auto& cameraComponent = entity.GetComponent<CameraComponent>();
            auto& camera = cameraComponent.Camera;

            out << YAML::Key << "Camera" << YAML::Value;
            out << YAML::BeginMap;  // Camera

            out << YAML::Key << "ProjectionType"   << YAML::Value << (int)camera.GetProjectionType();
            out << YAML::Key << "PerspectiveFOV"   << YAML::Value << camera.GetPerspectiveVerticalFOV();
            out << YAML::Key << "PerspectiveNear"  << YAML::Value << camera.GetPerspectiveNearClip();
            out << YAML::Key << "PerspectiveFar"   << YAML::Value << camera.GetPerspectiveFarClip();
            out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();
            out << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOrthographicNearClip();
            out << YAML::Key << "OrthographicFar"  << YAML::Value << camera.GetOrthographicFarClip();

            out << YAML::EndMap;    // Camera

            out << YAML::Key << "Primary"           << YAML::Value << cameraComponent.Primary;
            out << YAML::Key << "FixedAspectRatio"  << YAML::Value << cameraComponent.FixedAspectRatio;

            out << YAML::EndMap;    // Component
        }
        if (entity.HasComponent<SpriteComponent>())
        {
            out << YAML::Key << "SpriteComponent";
            out << YAML::BeginMap;  // Component

            auto& sprite = entity.GetComponent<SpriteComponent>();
            if (sprite.Texture)
                out << YAML::Key << "TexturePath" << YAML::Value << sprite.Texture->GetPath();
            out << YAML::Key << "Color" << YAML::Value << sprite.Color;
            out << YAML::Key << "Scale" << YAML::Value << sprite.Scale;

            out << YAML::EndMap;    // Component
        }
        if (entity.HasComponent<AudioSourceComponent>())
        {
            out << YAML::Key << "AudioSourceComponent";
            out << YAML::BeginMap;  // Component

            auto& audioSource = entity.GetComponent<AudioSourceComponent>();
            if (audioSource.Audio)
                out << YAML::Key << "AudioPath" << YAML::Value << audioSource.Audio->GetPath();
            out << YAML::Key << "Gain" << YAML::Value << audioSource.Gain;
            out << YAML::Key << "Pitch" << YAML::Value << audioSource.Pitch;
            out << YAML::Key << "Loop" << YAML::Value << audioSource.Loop;

            out << YAML::EndMap;    // Component
        }

        out << YAML::EndMap;    // Entity
    }

    bool SceneSerializer::Serialize(const std::string& filepath)
    {
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Scene"     << YAML::Value << "Untitled";
        out << YAML::Key << "Entities"  << YAML::Value << YAML::BeginSeq;
        
        m_Scene->m_Registry.Each([&](auto entityHandle) // TODO Bug when saving scenes changed entities
        {
            Entity entity = { entityHandle, m_Scene.get() };
            if (!m_Scene->m_Registry.IsAlive(entityHandle))
                return false;
            //LOG_TRACE("Start to serialize: %d", entityHandle);
            SerializeEntity(out, entity);
        });
        out << YAML::EndSeq;
        out << YAML::EndMap;

        std::ofstream fout(filepath);
        fout << out.c_str();
        return true;
    }

    bool SceneSerializer::SerializeRuntime(const std::string& filepath)
    {
        POL_CORE_FATAL("NOT IMPLEMENTED!!!");
        return false;
    }

    bool SceneSerializer::DeSerialize(const std::string& filepath)
    {
        std::ifstream stream(filepath);
        std::stringstream strStream;
        strStream << stream.rdbuf();

        YAML::Node data = YAML::Load(strStream.str());
        if (!data["Scene"])
            return false;

        std::string sceneName = data["Scene"].as<std::string>();
        POL_CORE_TRACE("Scene: Deseralizing scene: %s", sceneName.c_str());

        auto entities = data["Entities"];
        if (entities)
        {
            for (auto entity : entities)
            {
                uint64_t uuid = entity["Entity"].as<uint64_t>();

                std::string name;
                auto nameComponent = entity["NameComponent"];
                if (nameComponent)
                    name = nameComponent["Name"].as<std::string>();

                POL_CORE_TRACE("Scene: Deserialized %s: %d", name.c_str(), uuid);

                Entity newEntity = m_Scene->CreateEntity(name);

                auto transformComponent = entity["TransformComponent"];
                if (transformComponent)
                {
                    // entity always have a transform
                    auto& tc = newEntity.GetComponent<TransformComponent>();
                    tc.Position = transformComponent["Position"].as<glm::vec3>();
                    tc.Rotation = transformComponent["Rotation"].as<glm::vec3>();
                    tc.Scale    = transformComponent["Scale"].as<glm::vec3>();
                }

                auto cameraComponent = entity["CameraComponent"];
                if (cameraComponent)
                {
                    auto& cc = newEntity.AddComponent<CameraComponent>();

                    auto& cameraProps = cameraComponent["Camera"];
                    cc.Camera.SetProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<int>());

                    cc.Camera.SetPerspectiveVerticalFOV(cameraProps["PerspectiveFOV"].as<float>());
                    cc.Camera.SetPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
                    cc.Camera.SetPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());

                    cc.Camera.SetOrthographicSize(cameraProps["OrthographicSize"].as<float>());
                    cc.Camera.SetOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());
                    cc.Camera.SetOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());

                    cc.Primary = cameraComponent["Primary"].as<bool>();
                    cc.FixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
                }

                auto spriteComponent = entity["SpriteComponent"];
                if (spriteComponent)
                {
                    auto& sprite = newEntity.AddComponent<SpriteComponent>();

                    if (spriteComponent["TexturePath"])
                    {
                        std::string texturePath = spriteComponent["TexturePath"].as<std::string>();
                        auto path = Project::GetAssetDirectory() / texturePath;
                        sprite.Texture = Texture2D::Create(path.string());
                    }

                    sprite.Color = spriteComponent["Color"].as<glm::vec4>();

                    if (spriteComponent["Scale"])
                        sprite.Scale = spriteComponent["Scale"].as<float>();
                }

                auto audioSourceComponent = entity["AudioSourceComponent"];
                if (audioSourceComponent)
                {
                    auto& audioSource = newEntity.AddComponent<AudioSourceComponent>();
                    if (audioSourceComponent["AudioPath"])
                    {
                        std::string audioPath = audioSourceComponent["AudioPath"].as<std::string>();
                        auto path = Project::GetAssetDirectory() / audioPath;
                        audioSource.Audio = Audio::Create(path.string());
                    }

                    audioSource.Gain = audioSourceComponent["Gain"].as<float>();
                    audioSource.Pitch = audioSourceComponent["Pitch"].as<float>();
                    audioSource.Loop = audioSourceComponent["Loop"].as<bool>();
                }
            }
        }


        return true;
    }

    bool SceneSerializer::DeSerializeRuntime(const std::string& filepath)
    {
        POL_CORE_FATAL("NOT IMPLEMENTED!!!");
        return false;
    }
}
