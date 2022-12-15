namespace Flame
{
	// TODO
	// A wrapper of entt::registry

	class Registry final
	{
	public:
		Registry() = default;
		~Registry() = default;
	public:
	private:
		entt::registry m_Registry;
	};
}