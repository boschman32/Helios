#pragma once

namespace Helios
{
#define HIDE_FROM_INSPECTOR "HideFromInspector"
#define NO_SERIALIZE "No_Serialzation"
#define NO_DESERIALIZE "No_Deserialzation"

#define SET_HIDE_FROM_INSPECTOR rttr::metadata(HIDE_FROM_INSPECTOR, true)
#define SET_NO_DESERIALIZE rttr::metadata(NO_DESERIALIZE, true)
#define SET_NO_SERIALIZE rttr::metadata(NO_SERIALIZE, true)

#define S_TYPE "Type"

//Component
#define S_COMPONENT_NAME "Component"
#define S_COMPONENT_PROP_ID "id"
#define S_COMPONENT_PROP_OWNER_ID "ownerId"
#define S_COMPONENT_PROP_ENABLED "enabled"

//Component ID
#define S_COMPONENTID_NAME "ComponentID"
#define S_COMPONENTID_PROP_ID "id"

//QuadRenderer
#define S_QUAD_RENDERER_NAME "QuadRenderer"
#define S_QUAD_RENDERER_PROP_ORIGIN "Origin"
#define S_QUAD_RENDERER_PROP_TEXTURE "Texture"
#define S_QUAD_RENDERER_PROP_TEXTURE_ARCHIVE "TextureArchive"

//Vec2
#define S_VEC_2_NAME "Vec2"
#define S_VEC_2_PROP_X "x"
#define S_VEC_2_PROP_Y "y"
#define S_VEC_2_PROP_V "v"

//Transform
#define S_TRANSFORM_NAME "Transform"
#define S_TRANSFORM_PROP_POS "Position"
#define S_TRANSFORM_PROP_ROTATION "Rotation"
#define S_TRANSFORM_PROP_SCALE "Scale"
#define S_TRANSFORM_PROP_PARENT "Parent"
#define S_TRANSFORM_PROP_CHILDREN "Children"
#define S_TRANSFORM_PROP_LOCAL_POS "LocalPosition"
#define S_TRANSFORM_PROP_LOCAL_ROTATION "LocalRotation"

//AudioAsset
#define S_AUDIO_ASSET_NAME "AudioAsset"
#define S_AUDIO_ASSET_PROP_PATH "Filepath"
	
//AudioSource
#define S_AUDIO_SOURCE_NAME "Audio Source"
#define S_AUDIO_SOURCE_PROP_LOOPING "Loop"
#define S_AUDIO_SOURCE_PROP_STREAMING "Stream"
#define S_AUDIO_SOURCE_PROP_OFFSET "Decibel Offset"
#define S_AUDIO_SOURCE_PROP_ASSET "AudioAsset"

//LUA
#define S_LUA_NAME "Lua Script"
#define S_LUA_PROP_PATH "File Path"
//Collider
#define S_COLLIDER_NAME "Collider"
#define S_COLLIDER_PROP_VISIBLE "IsVisible"

//Circle Collider
#define S_CIRCLE_COLLIDER_NAME "Circle Collider"
#define S_CIRCLE_COLLIDER_PROP_RADIUS "Radius"
#define S_CIRCLE_COLLIDER_PROP_OFFSET "Offset"

//Polygon Collider
#define S_POLYGON_COLLIDER_NAME "Polygon Collider"
#define S_POLYGON_COLLIDER_PROP_SIZE "Size"
#define S_POLYGON_COLLIDER_PROP_OFFSET "Offset"
#define S_POLYGON_COLLIDER_PROP_OFFSET_ROTATION "Rotation Offset"

//Quad Collider
#define S_QUAD_COLLIDER_NAME "Quad Collider"
#define S_QUAD_COLLIDER_PROP_SIZE "Size"
#define S_QUAD_COLLIDER_PROP_OFFSET "Offset"
#define S_QUAD_COLLIDER_PROP_OFFSET_ROTATION "Rotation Offset"

//Entity
#define S_ENTITY_NAME "Entity"
#define S_ENTITY_PROP_NAME "Name"
#define S_ENTITY_PROP_ENABLED "Enabled"
#define S_ENTITY_PROP_ENTITY_ID "entityId"
#define S_ENTITY_PROP_COMPONENTS "Components"
#define S_ENTITY_PROP_HAS_PREFAB "HasPrefab"
#define S_ENTITY_PROP_PREFAB_PATH "PrefabPath"

//Entity-Id
#define S_ENTITYID_NAME "EntityId"
#define S_ENTITYID_PROP_ID "id"

//Scene
#define S_SCENE_NAME "Scene"
#define S_SCENE_PROP_NAME "Scene Name"
#define S_SCENE_PROP_PATH "Scene Path"
#define S_SCENE_PROP_ENTITIES "Entities"

//Resource
#define S_RESOURCE_NAME "Resource"

//Resource Archive
#define S_RESOURCE_ARCHIVE_NAME "ResourceArchive"
#define S_RESOURCE_ARCHIVE_PROP_TYPE "ResourceType"
#define S_RESOURCE_ARCHIVE_PROP_FILENAME "ResourceFilename"
#define S_RESOURCE_ARCHIVE_PROP_PATH "ResourcePath"
#define S_RESOURCE_ARCHIVE_PROP_NAME "ResourceName"

#define S_PREFAB_NAME "Prefab"
#define S_MESH_NAME "Mesh"

#define S_DX12TEXTURE_NAME "DX12Texture"

//Mesh Renderer
#define S_MESH_RENDERER_NAME "Mesh Renderer"
#define S_MESH_RENDERER_PROP_MESH "MeshAsset"
#define S_MESH_RENDERER_PROP_MATERIAL "MaterialAsset"
#define S_MESH_RENDERER_PROP_TEXTURE "DX12Texture"
#define S_MESH_RENDERER_PROP_TEXTURE_ARCHIVE "TextureArchive"

//Primitive Shape Component
#define S_PRIMITIVE_SHAPE_COMPONENT_NAME "Primitive Shape"
#define S_PRIMITIVE_SHAPE_PROP_SHAPE "CurrentShape"
#define S_PRIMITIVE_SHAPE_PROP_MATERIAL "MaterialAsset"
#define S_PRIMITIVE_SHAPE_PROP_TEXTURE "DX12Texture"
#define S_PRIMITIVE_SHAPE_PROP_TEXTURE_ARCHIVE "PrimitiveArchive"

//Defines for UI Component
#define S_UI_COMPONENT_NAME "UI"
#define S_UI_COMPONENT_PROP_TEXTURE "UI Texture"
#define S_UI_COMPONENT_ARCHIVE_NAME "UI TextureArchive"
#define S_UI_COMPONENT_PROP_POSITION "Position"
#define S_UI_COMPONENT_PROP_ANCHOR_POINT "Anchor Point"
#define S_UI_COMPONENT_PROP_SCALE "Scale"
#define S_UI_COMPONENT_PROP_ROTATION "Rotation"
}