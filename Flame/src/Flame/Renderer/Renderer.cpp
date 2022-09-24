#include "flamepch.h"
#include "Renderer.h"


namespace Flame {

	void Renderer::BeginScene()
	{
		//设置环境相关的信息（摄像机参数、环境光照
	}

	void Renderer::EndScene()
	{
		//优化：将使用相同材质的物体合并到一起、Cull Frustum、根据位置排序
	}

	void Renderer::Submit(const std::shared_ptr<VertexArray>& vertexArray)
	{
		//收集场景数据以及渲染命令，并将渲染命令加入队列，以便之后作优化
		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}
}