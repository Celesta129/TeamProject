#pragma once
class CComponent
{
public:
	CComponent();
	virtual ~CComponent();


public:
	bool m_bIsManagers = false;					// �� ������Ʈ �Ŵ����� 1. ������! ������ �����־����(IsManagers = true).
												// 2. ������Ʈ�������� IsManagers�� true�Ͻ� ����� ����� �ȵ�.

public:
	virtual CComponent* Clone(void) = 0;		// Clone ������ new Component(this)�� return this; �����Ұ�. ��, ���� ����-�����ؼ� �ѱ�Ͱ� �ڽ��� �����͸� �ѱ� ������Ʈ�� �����ؾ���.
												// �̷��� �����Ϸ��� ��Ȯ���� ���۷��� ī������ �ؼ� Release �Լ��� �����߰����� �ð��� ������. 
												// ������Ʈ �Ŵ����κ��� ������� ������Ʈ�� �����ִ� ������Ʈ�� �ڽ��� ������Ʈ ������ �ݵ��! �����͸� �����ִ°����� ���� ��������Ѱ����� �˼��ִ� bool���� üũ�ڵ带 �־����.
												// ���������� ���۷��� ī������ 100���� ������ �ٽ� �������� �ð��� ����

												// �� ������ 1. �Ŵ����� �����͸� �Ѱ��ִ� ��� IsManagers = true, return this;
												// 2. �Ŵ����� ���� ��������ؼ� �Ѱ��ִ°�� IsManagers = false, return new(Component); �ϰ���. ���� �� ����!
												
											

	
};

