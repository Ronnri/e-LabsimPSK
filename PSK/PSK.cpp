// PSK.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "PSK.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: ����� DLL ����� MFC DLL �Ƕ�̬���ӵģ�
//		��Ӵ� DLL �������κε���
//		MFC �ĺ������뽫 AFX_MANAGE_STATE ����ӵ�
//		�ú�������ǰ�档
//
//		����:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// �˴�Ϊ��ͨ������
//		}
//
//		�˺������κ� MFC ����
//		������ÿ��������ʮ����Ҫ������ζ��
//		��������Ϊ�����еĵ�һ�����
//		���֣������������ж������������
//		������Ϊ���ǵĹ��캯���������� MFC
//		DLL ���á�
//
//		�й�������ϸ��Ϣ��
//		����� MFC ����˵�� 33 �� 58��
//

// CPSKApp

BEGIN_MESSAGE_MAP(CPSKApp, CWinApp)
END_MESSAGE_MAP()


// CPSKApp ����

CPSKApp::CPSKApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CPSKApp ����

CPSKApp theApp;


// CPSKApp ��ʼ��

BOOL CPSKApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

//����һ���㷨���󣬲������㷨����ָ��
void *LtCreateObject()
{
	CAlgorithm *pAlgorithm = new CAlgorithm();
	//UserGui->Create(IDD_DIALOG1,NULL);
	return static_cast<void*>( pAlgorithm );
}

//ɾ��һ���㷨���󣬴˺����ڴ��Ĳ�����pObject������LtCreateObject()�ķ���ֵ
void LtDestroyObject( void * pObject )
{
	ASSERT( pObject != NULL );
	ASSERT( !IsBadWritePtr( pObject, sizeof(CAlgorithm) ) );
	CAlgorithm * pAlgorithm = static_cast<CAlgorithm *>( pObject );
	//pAlgorithm->DestroyWindow();
	delete pAlgorithm;	//ɾ��һ���㷨����
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
	//PSK����-����Ϊ�ߵ�ƽʱ���ͬ���ز�������Ϊ�͵�ƽʱ��������ز�
	if( pdInput[IN1] > 1.0 )
		pdOutput[OUT1] = pdInput[IN2];
	else
		pdOutput[OUT1] = -pdInput[IN2];

	//PSK���
	double DemodulationData = pdInput[IN3]*pdInput[IN4];
	FIR_Counter++;
	if( FIR_Counter > 127 )
	{
		FIR_Counter = 0;
		pdOutput[OUT2] = LowPass( DemodulationData );
	}
	pdOutput[OUT3] = CompareOut( pdInput[IN5], pdInput[W1] );
}

//����Ƶ��1024K,32K��ͨ�˲���
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
