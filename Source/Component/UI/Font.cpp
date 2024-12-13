#include "Font.h"
#include <WICTextureLoader.h>
#include "SystemStruct\Targa.h"
#include "SystemStruct\Misc.h"
#include "Graphics/Graphics.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include <string>
#include <codecvt>
#include<windows.h>

// UTF-16 (std::wstring) �� UTF-8 (std::string) �ϊ�
std::string WStringToUTF8(const std::wstring& wstr) {
	std::string result;
	result.reserve(wstr.size() * 4); // UTF-8�ł�1�������ő�4�o�C�g�ɂȂ�
	for (wchar_t wc : wstr) {
		if (wc <= 0x7F) {
			result.push_back(static_cast<char>(wc)); // ASCII
		}
		else if (wc <= 0x7FF) {
			result.push_back(static_cast<char>(0xC0 | ((wc >> 6) & 0x1F)));
			result.push_back(static_cast<char>(0x80 | (wc & 0x3F)));
		}
		else if (wc <= 0xFFFF) {
			result.push_back(static_cast<char>(0xE0 | ((wc >> 12) & 0x0F)));
			result.push_back(static_cast<char>(0x80 | ((wc >> 6) & 0x3F)));
			result.push_back(static_cast<char>(0x80 | (wc & 0x3F)));
		}
		else {
			result.push_back(static_cast<char>(0xF0 | ((wc >> 18) & 0x07)));
			result.push_back(static_cast<char>(0x80 | ((wc >> 12) & 0x3F)));
			result.push_back(static_cast<char>(0x80 | ((wc >> 6) & 0x3F)));
			result.push_back(static_cast<char>(0x80 | (wc & 0x3F)));
		}
	}
	return result;
}

// UTF-8 (std::string) �� UTF-16 (std::wstring) �ϊ�
std::wstring UTF8ToWString(const std::string& str) {
	std::wstring result;
	size_t i = 0;
	while (i < str.size()) {
		unsigned char c = str[i];
		if (c <= 0x7F) {
			result.push_back(c);
			++i;
		}
		else if ((c & 0xE0) == 0xC0) {
			wchar_t wc = ((c & 0x1F) << 6) | (str[i + 1] & 0x3F);
			result.push_back(wc);
			i += 2;
		}
		else if ((c & 0xF0) == 0xE0) {
			wchar_t wc = ((c & 0x0F) << 12) | ((str[i + 1] & 0x3F) << 6) | (str[i + 2] & 0x3F);
			result.push_back(wc);
			i += 3;
		}
		else if ((c & 0xF8) == 0xF0) {
			wchar_t wc = ((c & 0x07) << 18) | ((str[i + 1] & 0x3F) << 12) | ((str[i + 2] & 0x3F) << 6) | (str[i + 3] & 0x3F);
			result.push_back(wc);
			i += 4;
		}
		else {
			// �s����UTF-8�f�[�^�𖳎�����
			++i;
		}
	}
	return result;
}

// ImGui��std::wstring�������֐�
bool InputTextWString(const char* label, std::wstring& wstr, size_t max_length = 256) {
	// wstring��UTF-8�ɕϊ�
	std::string utf8 = WStringToUTF8(wstr);

	// �o�b�t�@������
	std::vector<char> buffer(max_length + 1); // NULL�I�[�̕����l��
	strncpy_s(buffer.data(), buffer.size(), utf8.c_str(), max_length);

	// ImGui��InputText�ŕҏW
	bool edited = ImGui::InputText(label, buffer.data(), buffer.size());

	if (edited) {
		// �ҏW��̕������UTF-8����wstring�ɕϊ�
		wstr = UTF8ToWString(std::string(buffer.data()));
	}

	return edited;
}


Font::Font( const char* filename, int maxSpriteCount)
{

	//Font�̍�����PVP�V�[���ɃC�j�V�����C�Y�ɎQ�l��u���Ƃ��܂�
	HRESULT hr = S_OK;

	ID3D11Device* device = Graphics::Instance().GetDevice();
	ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();

	// ���_�V�F�[�_�[
	{
		
		// ���_�V�F�[�_�[
		{
			//���̓��C�A�E�g
			D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
			{
		    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "MASK",     0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};
			CreateVsFromCso(device, "Shader\\Font_VS.cso", vertexShader.GetAddressOf(), inputLayout.GetAddressOf(), inputElementDesc, _countof(inputElementDesc));
		}

	}
	// �s�N�Z���V�F�[�_�[
	{	
	  CreatePsFromCso(device, "Shader\\Font_PS.cso", pixelShader.GetAddressOf());
	}

	// ���_�o�b�t�@
	{
		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * maxSpriteCount * 4);
		//bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;

		HRESULT hr = device->CreateBuffer(&bufferDesc, nullptr, vertexBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// �C���f�b�N�X�o�b�t�@
	{
		D3D11_BUFFER_DESC bufferDesc = {};
		D3D11_SUBRESOURCE_DATA subresourceData = {};

		std::unique_ptr<UINT[]> indices = std::make_unique<UINT[]>(maxSpriteCount * 6);
		// �l�p�` �� �O�p�` �Q�ɓW�J
		// 0---1      0---1  4
		// |   |  ��  |�^  �^|
		// 2---3      2  3---5
		{
			UINT* p = indices.get();
			for (int i = 0; i < maxSpriteCount * 4; i += 4)
			{
				p[0] = i + 0;
				p[1] = i + 1;
				p[2] = i + 2;
				p[3] = i + 2;
				p[4] = i + 3;
				p[5] = i + 1;
				p += 6;
			}
		}

		bufferDesc.ByteWidth = static_cast<UINT>(sizeof(UINT) * maxSpriteCount * 6);
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;
		subresourceData.pSysMem = indices.get();
		subresourceData.SysMemPitch = 0; //Not use for index buffers.
		subresourceData.SysMemSlicePitch = 0; //Not use for index buffers.
		HRESULT hr = device->CreateBuffer(&bufferDesc, &subresourceData, indexBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// .font �ǂݍ���
	{
		FILE* fp = nullptr;
		fopen_s(&fp, filename, "rb");
		_ASSERT_EXPR_A(fp, "FONT File not found");

		fseek(fp, 0, SEEK_END);
		long fontSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		std::unique_ptr<UINT8[]> fontData = std::make_unique<UINT8[]>(fontSize);
		fread(fontData.get(), fontSize, 1, fp);
		fclose(fp);

		//
		// +-------------------------------------------------------+
		// | Font::Header                                          |
		// +-------------------------------------------------------+
		// | char[256] * Font::Header::textureNum                  |
		// +-------------------------------------------------------+
		// | UINT16 * 65536                                        |
		// +-------------------------------------------------------+
		// | Font::CharacterInfo * Font::Header::characterInfoNum  |
		// +-------------------------------------------------------+
		UINT8* p = fontData.get();

		// �w�b�_
		{
			Header* header = reinterpret_cast<Header*>(p);
			p += sizeof(Header);

			fontWidth = static_cast<float>(header->fontWidth);
			fontHeight = static_cast<float>(header->fontHeight);
			textureCount = header->textureNum;
			characterCount = header->characterInfoNum;
		}

		// �e�N�X�`���t�@�C��
		{
			shaderResourceViews.resize(textureCount);

			// �f�B���N�g���p�X�擾
			char dirname[256];
			::_splitpath_s(filename, nullptr, 0, dirname, 256, nullptr, 0, nullptr, 0);

			for (int i = 0; i < textureCount; ++i)
			{
				// �e�N�X�`���t�@�C����
				char* filename = reinterpret_cast<char*>(p);
				p += 256;

				// ���΃p�X�̉���
				char fname[256];
				::_makepath_s(fname, 256, nullptr, dirname, filename, nullptr);

				// �e�N�X�`���ǂݍ���
				{
					fopen_s(&fp, fname, "rb");
					_ASSERT_EXPR_A(fp, "TGA File not found");

					fseek(fp, 0, SEEK_END);
					long tgaSize = ftell(fp);
					fseek(fp, 0, SEEK_SET);

					std::unique_ptr<UINT8[]> tgaData = std::make_unique<UINT8[]>(tgaSize);
					fread(tgaData.get(), tgaSize, 1, fp);
					fclose(fp);

					// TGA�f�[�^�̓W�J
					TargaHeader* tgaHeader = reinterpret_cast<TargaHeader*>(tgaData.get());

					DXGI_FORMAT format;
					switch (tgaHeader->pixelDepth)
					{
					case 8:
						format = DXGI_FORMAT_R8_UNORM;
						break;
					case 16:
						format = DXGI_FORMAT_B5G6R5_UNORM;
						break;
					case 32:
						format = DXGI_FORMAT_R8G8B8A8_UNORM;// DXGI_FORMAT_B8G8R8A8_UNORM;
						break;
					}

					// �摜�f�[�^�̎��o��
					int lineSize = (tgaHeader->width * tgaHeader->pixelDepth) / 8;
					int imageSize = lineSize * tgaHeader->height;
					std::unique_ptr<UINT8[]> imageData = std::make_unique<UINT8[]>(imageSize);

					UINT8* src = reinterpret_cast<UINT8*>(&tgaHeader[1]);
					switch (tgaHeader->imageDescriptor & (TGA_ATTR_UP2DOWN | TGA_ATTR_RIGHT2LEFT))
					{
						// ������E�A�ォ�牺
					case TGA_ATTR_UP2DOWN:
					{
						UINT8* dst = imageData.get();
						for (int i = tgaHeader->height; i > 0; --i)
						{
							memcpy(dst, src, lineSize);
							dst += lineSize;
							src += lineSize;
						}
					}
					break;

					// ������E�A�������
					default:
					{
						UINT8* dst = imageData.get() + lineSize * (tgaHeader->height - 1);
						for (int i = tgaHeader->height; i > 0; --i)
						{
							memcpy(dst, src, lineSize);
							dst -= lineSize;
							src += lineSize;
						}
					} break;
					}

					// �e�N�X�`���쐬
					D3D11_SUBRESOURCE_DATA subresourceData;
					memset(&subresourceData, 0, sizeof(subresourceData));
					subresourceData.pSysMem = imageData.get();
					subresourceData.SysMemPitch = lineSize;

					D3D11_TEXTURE2D_DESC desc = { 0 };
					desc.Width = tgaHeader->width;
					desc.Height = tgaHeader->height;
					desc.MipLevels = 1;
					desc.ArraySize = 1;
					desc.Format = format;
					desc.SampleDesc.Count = 1;
					desc.SampleDesc.Quality = 0;
					desc.Usage = D3D11_USAGE_DEFAULT;
					desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
					desc.CPUAccessFlags = 0;
					desc.MiscFlags = 0;

					ID3D11Texture2D* texture2d;
					HRESULT hr = device->CreateTexture2D(&desc, &subresourceData, &texture2d);
					assert(SUCCEEDED(hr));

					hr = device->CreateShaderResourceView(texture2d, nullptr, shaderResourceViews.at(i).GetAddressOf());
					assert(SUCCEEDED(hr));
					texture2d->Release();
				}
			}
		}

		// �����R�[�h�e�[�u��
		{
			characterIndices.resize(65536);
			::memcpy(characterIndices.data(), p, sizeof(UINT16) * 65536);
			p += sizeof(UINT16) * 65536;
		}

		// �������
		{
			characterInfos.resize(characterCount);
			::memcpy(characterInfos.data(), p, sizeof(CharacterInfo) * characterCount);
		}
	}
}

void Font::Render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
	Graphics& Graphics = Graphics::Instance();
	ID3D11Device* device = Graphics.GetDevice();
	ID3D11DeviceContext* dc = Graphics.GetDeviceContext();

	 currentPosition = position;

	// �X�N���[���T�C�Y�擾
	D3D11_VIEWPORT viewport;
	UINT num_viewports = 1;
	dc->RSGetViewports(&num_viewports, &viewport);
	screenWidth = viewport.Width;
	screenHeight = viewport.Height;

	// �X�e�[�g�̐ݒ�
	dc->OMSetBlendState(Graphics.GetBlendState(BLENDSTATE::ALPHA), nullptr, 0xFFFFFFFF);
	dc->OMSetDepthStencilState(Graphics.GetDepthStencilState(DEPTHSTATE::ZT_ON_ZW_ON), 1);
	dc->RSSetState(Graphics.GetRasterizerState(RASTERIZERSTATE::SOLID_CULL_NONE));

	stri = L"���Ƃ���������";
	// ���_�ҏW�J�n
	D3D11_MAPPED_SUBRESOURCE mapped_subresource;
	dc->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);

	currentVertex = reinterpret_cast<Vertex*>(mapped_subresource.pData);
	currentIndexCount = 0;
	currentPage = -1;
	subsets.clear();

	size_t length = str.size();
	//size_t length = ::wcslen(stri);

	float start_x = currentPosition.x;
	float start_y = currentPosition.y;
	float space = fontWidth;

	for (size_t i = 0; i < length; ++i)
	{
		// �����l���當����񂪊i�[����Ă���R�[�h���擾
		UINT16 word = static_cast<UINT16>(str[i]);
		UINT16 code = characterIndices.at(word);

		// ���ꐧ��p�R�[�h�̏���
		if (code == CharacterInfo::EndCode)
		{
			break;
		}
		else if (code == CharacterInfo::ReturnCode)
		{
			currentPosition.x = start_x;
			currentPosition.y += fontHeight;
			continue;
		}
		else if (code == CharacterInfo::TabCode)
		{
			currentPosition.x += space * 4;
			continue;
		}
		else if (code == CharacterInfo::SpaceCode)
		{
			currentPosition.x += space;
			continue;
		}

		// �}�X�N
		static const DirectX::XMFLOAT4 mask[4] =
		{
			DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f),
			DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f),
			DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 0.0f),
			DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
		};

		// ���������擾���A���_�f�[�^��ҏW
		const CharacterInfo& info = characterInfos.at(code);

		float positionX = currentPosition.x + info.xoffset * scale;// + 0.5f;
		float positionY = currentPosition.y + info.yoffset * scale;// + 0.5f;
		float width = info.width * scale;
		float height = info.height * scale;

		// 0---1
		// |   |
		// 2---3
		currentVertex[0].position.x = positionX;
		currentVertex[0].position.y = positionY;
		currentVertex[0].position.z = 0.0f;
		currentVertex[0].texcoord.x = info.left;
		currentVertex[0].texcoord.y = info.top;
		currentVertex[0].color.x = 1.0f;
		currentVertex[0].color.y = 1.0f;
		currentVertex[0].color.z = 1.0f;
		currentVertex[0].color.w = 1.0f;
		currentVertex[0].mask = mask[info.channel];

		currentVertex[1].position.x = positionX + width;
		currentVertex[1].position.y = positionY;
		currentVertex[1].position.z = 0.0f;
		currentVertex[1].texcoord.x = info.right;
		currentVertex[1].texcoord.y = info.top;
		currentVertex[1].color.x = 1.0f;
		currentVertex[1].color.y = 1.0f;
		currentVertex[1].color.z = 1.0f;
		currentVertex[1].color.w = 1.0f;
		currentVertex[1].mask = mask[info.channel];

		currentVertex[2].position.x = positionX;
		currentVertex[2].position.y = positionY + height;
		currentVertex[2].position.z = 0.0f;
		currentVertex[2].texcoord.x = info.left;
		currentVertex[2].texcoord.y = info.bottom;
		currentVertex[2].color.x = 1.0f;
		currentVertex[2].color.y = 1.0f;
		currentVertex[2].color.z = 1.0f;
		currentVertex[2].color.w = 1.0f;
		currentVertex[2].mask = mask[info.channel];

		currentVertex[3].position.x = positionX + width;
		currentVertex[3].position.y = positionY + height;
		currentVertex[3].position.z = 0.0f;
		currentVertex[3].texcoord.x = info.right;
		currentVertex[3].texcoord.y = info.bottom;
		currentVertex[3].color.x = 1.0f;
		currentVertex[3].color.y = 1.0f;
		currentVertex[3].color.z = 1.0f;
		currentVertex[3].color.w = 1.0f;
		currentVertex[3].mask = mask[info.channel];

		// NDC���W�ϊ�
		for (int j = 0; j < 4; ++j)
		{
			currentVertex[j].position.x = 2.0f * currentVertex[j].position.x / screenWidth - 1.0f;
			currentVertex[j].position.y = 1.0f - 2.0f * currentVertex[j].position.y / screenHeight;
		}
		currentVertex += 4;

		currentPosition.x+= info.xadvance * scale;
		
		// �e�N�X�`�����؂�ւ��x�ɕ`�悷�����ݒ�
		if (currentPage != info.page)
		{
			currentPage = info.page;

			Subset subset;
			subset.shaderResourceView = shaderResourceViews.at(info.page).Get();
			subset.startIndex = currentIndexCount;
			subset.indexCount = 0;
			subsets.emplace_back(subset);
		}
		currentIndexCount += 6;
	}
	dc->Unmap(vertexBuffer.Get(), 0);

	// �T�u�Z�b�g����
	size_t size = subsets.size();
	for (size_t i = 1; i < size; ++i)
	{
		Subset& prev = subsets.at(i - 1);
		Subset& next = subsets.at(i);
		prev.indexCount = next.startIndex - prev.startIndex;
	}
	if (subsets.size() <= 0)return;
	Subset& last = subsets.back();
	last.indexCount = currentIndexCount - last.startIndex;


	// �V�F�[�_�[�ݒ�
	dc->VSSetShader(vertexShader.Get(), nullptr, 0);
	dc->PSSetShader(pixelShader.Get(), nullptr, 0);
	dc->IASetInputLayout(inputLayout.Get());
	
	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	dc->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	dc->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
;

	// �`��
	for (size_t i = 0; i < size; ++i)
	{
		Subset& subset = subsets.at(i);

		dc->PSSetShaderResources(0, 1, &subset.shaderResourceView);
		dc->DrawIndexed(subset.indexCount, subset.startIndex, 0);
	}
}

void Font::OnGUI()
{
	if (InputTextWString("String Input", str)) {
		// ���[�U�[��Enter���������Ƃ��A�����񂪍X�V�����
		ImGui::Text("Updated: %ls", str.c_str());
	}

	ImGui::DragFloat2("Position", &position.x);
	ImGui::DragFloat("Scale", &scale);
}