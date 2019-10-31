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
			static unsigned long DestroyInstance(void);									\

#define _IMPLEMENT_SINGLETON(CLASSNAME)													\
CLASSNAME* CLASSNAME::m_pInstance = nullptr;											\
CLASSNAME* CLASSNAME::GetInstance(void){												\
	if (m_pInstance == nullptr) {														\
		m_pInstance = new CLASSNAME;													\
	}																					\
	return m_pInstance;																	\
}																						\
unsigned long CLASSNAME::DestroyInstance(void) {										\
	unsigned long dwRefCnt = 0;															\
	if (nullptr != m_pInstance) {														\
		dwRefCnt = m_pInstance->Release();												\
		if (0 == dwRefCnt)																\
			m_pInstance = nullptr;														\
	}																					\
	return dwRefCnt;																	\
}

#define _BEGIN(Namespace) namespace Namespace{
#define _END }
#define _USING(Namespace) using namespace Namespace;