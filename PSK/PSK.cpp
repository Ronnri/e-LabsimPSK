// PSK.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "PSK.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: 如果此 DLL 相对于 MFC DLL 是动态链接的，
//		则从此 DLL 导出的任何调入
//		MFC 的函数必须将 AFX_MANAGE_STATE 宏添加到
//		该函数的最前面。
//
//		例如:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 此处为普通函数体
//		}
//
//		此宏先于任何 MFC 调用
//		出现在每个函数中十分重要。这意味着
//		它必须作为函数中的第一个语句
//		出现，甚至先于所有对象变量声明，
//		这是因为它们的构造函数可能生成 MFC
//		DLL 调用。
//
//		有关其他详细信息，
//		请参阅 MFC 技术说明 33 和 58。
//

// CPSKApp

BEGIN_MESSAGE_MAP(CPSKApp, CWinApp)
END_MESSAGE_MAP()


// CPSKApp 构造

CPSKApp::CPSKApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CPSKApp 对象

CPSKApp theApp;


// CPSKApp 初始化

BOOL CPSKApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

//创建一个算法对象，并返回算法对象指针
void *LtCreateObject()
{
	CAlgorithm *pAlgorithm = new CAlgorithm();
	//UserGui->Create(IDD_DIALOG1,NULL);
	return static_cast<void*>( pAlgorithm );
}

//删除一个算法对象，此函数期待的参数（pObject）正是LtCreateObject()的返回值
void LtDestroyObject( void * pObject )
{
	ASSERT( pObject != NULL );
	ASSERT( !IsBadWritePtr( pObject, sizeof(CAlgorithm) ) );
	CAlgorithm * pAlgorithm = static_cast<CAlgorithm *>( pObject );
	//pAlgorithm->DestroyWindow();
	delete pAlgorithm;	//删除一个算法对象
}

void LtDLLMain(	void * pObject, const bool *pbIsPortUsing, const double *pdInput, double *pdOutput, const int nSimuStep )
{
	ASSERT( pObject != NULL );
	CAlgorithm * pAlgorithm = static_cast<CAlgorithm *>( pObject );
	pAlgorithm->RunAlgorithm( pdInput, pdOutput );
}

void LtResetModule( void *pObject )
{
	ASSERT( pObject != NULL );
	ASSERT( !IsBadWritePtr( pObject, sizeof(CAlgorithm) ) );
	CAlgorithm * pAlgorithm = static_cast<CAlgorithm *>( pObject );
	pAlgorithm->Reset();
}

CAlgorithm::CAlgorithm()
{
	Reset();
}

CAlgorithm::~CAlgorithm()
{
}

void CAlgorithm::Reset()
{
	FIR_Counter = 0;
	memset( m_LowPass_Buffer, 0, sizeof( m_LowPass_Buffer ) );
	m_LowPass_Counter = 0;
	m_LowPass_Result = 0;
}

void CAlgorithm::RunAlgorithm(const double *pdInput, double *pdOutput)
{
	//PSK调制-数据为高电平时输出同向载波，数据为低电平时输出反向载波
	if( pdInput[IN1] > 1.0 )
		pdOutput[OUT1] = pdInput[IN2];
	else
		pdOutput[OUT1] = -pdInput[IN2];

	//PSK解调
	double DemodulationData = pdInput[IN3]*pdInput[IN4];
	FIR_Counter++;
	if( FIR_Counter > 127 )
	{
		FIR_Counter = 0;
		pdOutput[OUT2] = LowPass( DemodulationData );
	}
	pdOutput[OUT3] = CompareOut( pdInput[IN5], pdInput[W1] );
}

//工作频率1024K,32K低通滤波器
double CAlgorithm::LowPass(const double DataIn)
{
	m_LowPass_Buffer[m_LowPass_Counter] = DataIn;
	m_LowPass_Counter++;
	m_LowPass_Counter %= BL;
	m_LowPass_Result = 0;
	for( int i = 0; i < BL; ++i )
	{
		m_LowPass_Result = m_LowPass_Result + (m_LowPass_Buffer[(m_LowPass_Counter+i)%BL]) * H_LowPass32K[i];
	}
	return m_LowPass_Result;
}

double CAlgorithm::CompareOut(const double DataIn, const double Param)
{
	if( DataIn > Param )
		return 3.3;
	else
		return 0;
}
