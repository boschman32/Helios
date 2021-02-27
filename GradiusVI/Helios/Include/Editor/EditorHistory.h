#pragma once
namespace Helios
{
	class EditorHistory
	{
	public:
		struct EditorChange
		{
			enum class EChangeType
			{
				CHANGE_TYPE_COMPONENT, CHANGE_TYPE_ENTITY
			};
			EChangeType m_changeType;
			std::string m_oldValue;
			std::string m_newValue;
		};
		static void AddChange(EditorChange::EChangeType a_type, const std::string& a_oldValue, const std::string& a_newValue);
		static void UndoChange();
		static void RedoChange();
		static void ClearHistory();

		static std::deque<EditorChange> m_undoList;
		static std::deque<EditorChange> m_redoList;
	private:
		static bool CanModifyComponent(const std::string& a_inputString);
		static void RemoveComponent(const std::string& a_inputString);
		static bool CanModifyEntity(const std::string& a_inputString);
		static void RemoveEntity(const std::string& a_inputString);
	};
}

