// Library: OpenSiv3D

struct MandelbrotSetParam
{
	int g_opCount;

	long double g_offsetX;
	long double g_offsetY;

	long double g_size;

	int g_resolusionX;
	int g_resolusionY;
};

void Main()
{
	Scene::SetResizeMode(ResizeMode::Keep);
	Scene::SetTextureFilter(TextureFilter::Nearest);
	Scene::Resize(1500, 1000);

	Window::SetStyle(WindowStyle::Sizable);

	const PixelShader ps = GLSL{ U"mandelbrot_set.frag", { { U"PSConstants2D", 0 }, { U"MandelbrotSetParam", 1 } } };

	// parameters

	const long double defaultSize = 4;
	long double lastSize = defaultSize;
	long double moveValue = 0.5;
	Vector2D<long double> lastOffset = {};
	int opCount = 25;

	RenderTexture tex{ 1000, 1000 };

	const Font UIFont{ 32 };

	auto updateMandelbrot = [&]()
	{
		tex.clear(Palette::Black);

		const ScopedRenderTarget2D rtarg{ tex };

		const ScopedCustomShader2D cshad{ ps };

		Graphics2D::SetPSConstantBuffer
		(1
		, ConstantBuffer<MandelbrotSetParam>{ MandelbrotSetParam{ opCount, lastOffset.x, lastOffset.y, lastSize, tex.size().x, tex.size().y } }
		);

		Rect(tex.size()).draw(Palette::White);
	};

	updateMandelbrot();

	while (System::Update())
	{
		// key input
		if (KeyW.down()) { moveValue += 0.1; }
		if (KeyS.down()) { moveValue -= 0.1; }

		if (KeyE.down()) { ++opCount; }
		if (KeyD.down()) { opCount = Max(0, opCount - 1); }

		if (Key3.pressed()) { ++opCount; }
		if (KeyC.pressed()) { opCount = Max(0, opCount - 1); }

		if (KeyQ.down()) { lastSize /= 2; updateMandelbrot(); }
		if (KeyA.down()) { lastSize *= 2; updateMandelbrot(); }

		if (KeyLeft .down()) { lastOffset.x -= moveValue * (lastSize / defaultSize); updateMandelbrot(); }
		if (KeyRight.down()) { lastOffset.x += moveValue * (lastSize / defaultSize); updateMandelbrot(); }
		if (KeyUp   .down()) { lastOffset.y -= moveValue * (lastSize / defaultSize); updateMandelbrot(); }
		if (KeyDown .down()) { lastOffset.y += moveValue * (lastSize / defaultSize); updateMandelbrot(); }

		//if (KeyZ.down()) { if (++currentColor == colorTable.end()) currentColor = colorTable.begin(); }

		if (KeySpace.down()) { updateMandelbrot(); }

		// draw mandelbrot set

		{
			const ScopedRenderStates2D rstat{ SamplerState::ClampNearest };

			tex.draw(Arg::topRight = Scene::Rect().tr());
		}

		// draw UI
		{
			Vec2 pos = {};

			pos.y = UIFont(U"[arrow keys]: move"                                     ).draw(pos).bl().y;
			pos.y = UIFont(U"[Q, A]: zoom ({0:f})"_fmt(1 / (lastSize / defaultSize)) ).draw(pos).bl().y;
			pos.y = UIFont(U"[W, S]: change move value ({0:f})"_fmt(moveValue)       ).draw(pos).bl().y;
			pos.y = UIFont(U"[E, D]: change num operation ({0})"_fmt(opCount)        ).draw(pos).bl().y;
			pos.y = UIFont(U"[3, C]: change num operation(smooth) ({0})"_fmt(opCount)).draw(pos).bl().y;
			pos.y = UIFont(U"[space]: update"                                        ).draw(pos).bl().y;
			//pos.y = UIFont(U"[Z]: change color ({})"_fmt(currentColor->first)        ).draw(pos).bl().y;
			//pos.y = UIFont(U"operation time: {}"_fmt(Format(lastOperationTime))      ).draw(pos).bl().y;
		}

	}
}
