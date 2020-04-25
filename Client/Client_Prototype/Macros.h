#pragma once

#define _MSG_BOX(Message) MessageBox(0,TEXT(Message),TEXT("System Message"),MB_OK);

#define NO_COPY(CLASSNAME)																\
private:																				\
	CLASSNAME(const CLASSNAME&);														\
	CLASSNAME& operator = (const CLASSNAME&);											\

#define _DECLARE_SINGLETON(CLASSNAME)													\
	NO_COPY(CLASSNAME)																	\
		private:																		\
			static CLASSNAME* m_pInstance;												\
		public:																			\
			static CLASSNAME* GetInstance(void);										\
			static int		  DestroyInstance(void);									\

#define _IMPLEMENT_SINGLETON(CLASSNAME)													\
		CLASSNAME* CLASSNAME::m_pInstance = nullptr;									\
		CLASSNAME* CLASSNAME::GetInstance(void){										\
		if (m_pInstance == nullptr) {													\
			m_pInstance = new CLASSNAME;												\
		}																				\
		return m_pInstance;																\
}																						\
		int CLASSNAME::DestroyInstance( void ) {										\
			int RefCnt = 0;																\
		if(NULL != m_pInstance)	{														\
			RefCnt = m_pInstance->Release();											\
			if(0 > RefCnt) m_pInstance = NULL;											\
		}																				\
		return RefCnt;																	\
	}

#define _BEGIN(Namespace) namespace Namespace{
#define _END }
#define _USING(Namespace) using namespace Namespace;


#define GET_COMPNENT(Class, pObject, String)  (Class)(pObject)->Get_Component(String);