//	Copyright (c) 2008-2022 Ryo Suzuki.
//	Copyright (c) 2016-2022 OpenSiv3D Project.
//	Licensed under the MIT License.
// を改変してこのファイルは作成されました。

# version 410

//
//	PSInput
//
layout(location = 0) in vec4 Color;

//
//	PSOutput
//
layout(location = 0) out vec4 FragColor;

//
//	Constant Buffer
//
layout(std140) uniform PSConstants2D
{
	vec4 g_colorAdd;
	vec4 g_sdfParam;
	vec4 g_sdfOutlineColor;
	vec4 g_sdfShadowColor;
	vec4 g_internal;
};

layout(std140) uniform MandelbrotSetParam
{
	int g_opCount;

	highp double g_offsetX;
	highp double g_offsetY;

	highp double g_size;

	int g_resolusionX;
	int g_resolusionY;
};

//
//	Functions
//
void main()
{
	int px = int(gl_FragCoord.x);
	int py = int(gl_FragCoord.y);

	highp double a = px * g_size / g_resolusionX - g_size / 2 + g_offsetX;
	highp double b = py * g_size / g_resolusionY - g_size / 2 + g_offsetY;

	highp double x = 0;
	highp double y = 0;

	vec4 color = Color;

	for (int i = 0; i < g_opCount; ++i)
	{
		highp double new_x = x * x - y * y + a;
		highp double new_y = 2 * x * y + b;

		x = new_x;
		y = new_y;

		if (x * x + y * y > 4)
		{
			float v = float(i) / g_opCount;
			color = mix(vec4(0, 0, 0, 1), Color, v);
			break;
		}
	}

	FragColor = (color + g_colorAdd);
}
