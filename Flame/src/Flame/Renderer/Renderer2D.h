#pragma once

#include "OrthographicCamera.h"

namespace Flame {

	class Renderer2D
	{
	public:
		// ��������Ϊstatic�ĺô���ʲô��
		/*����ȫ�����Ǿ�̬������֮���Բ����ɳ�Ա��������Ϊû�б�Ҫ���Ͼ���Ա����������Ҳ�Ǿ�̬������
		�޷Ǿ�̬�����ĵ�һ�����������thisָ�����*/
		static void Init();
		static void Shutdown();

		static void BeginScene(const OrthographicCamera& camera);
		static void EndScene();

		// Primitives
		//vec2��vec3��position��Ϊ�˼���depth��Ϣ
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
	};

}