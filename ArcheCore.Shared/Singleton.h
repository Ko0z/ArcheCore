#ifndef _SINGLETON_H
#define _SINGLETON_H

namespace ArcheCore
{
	template<typename T>
	class Singleton
	{
	public:
		static T* Instance();
		static void Destroy();

	private:

		Singleton(Singleton const&) {};
		Singleton& operator=(Singleton const&) {};

	protected:
		static T* m_instance;

		Singleton() { m_instance = static_cast <T*> (this); };
		~Singleton() {};
	};

	template<typename T>
	typename T* Singleton<T>::m_instance = 0;

	template<typename T>
	T* Singleton<T>::Instance()
	{
		if (!m_instance)
		{
			Singleton<T>::m_instance = new T();
		}

		return m_instance;
	}

	template<typename T>
	void Singleton<T>::Destroy()
	{
		delete Singleton<T>::m_instance;
		Singleton<T>::m_instance = 0;
	}
}

#endif