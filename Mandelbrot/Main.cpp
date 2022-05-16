// Library: OpenSiv3D

Image CreateMandelbrotImg
( const Vec2& offset
, double size
, const Size& resolusion
, int opCount
, const std::function<ColorF(double)>& color
)
{
	Image img{ resolusion, Palette::White };

	for (int py = 0; py < resolusion.y; ++py)
	{
		const long double b = py * size / resolusion.y - size / 2 + offset.y;

		for (int px = 0; px < resolusion.x; ++px)
		{
			const long double a = px * size / resolusion.x - size / 2 + offset.x;

			long double x = 0, y = 0;

			for (int i = 0; i < opCount; ++i)
			{
				const auto new_x = x * x - y * y + a;
				const auto new_y = 2 * x * y + b;

				x = new_x;
				y = new_y;

				if (x * x + y * y > 4)
				{
					img[Point{ px, py }] = color(i / static_cast<double>(opCount));
					break;
				}
			}
		}
	}

	return img;
}

void Main()
{
	Scene::SetResizeMode(ResizeMode::Keep);
	Scene::SetTextureFilter(TextureFilter::Nearest);
	Scene::Resize(1500, 1000);

	Window::SetStyle(WindowStyle::Sizable);

	// parameters

	const double defaultSize = 4;
	double lastSize = defaultSize;
	double moveValue = 0.5;
	Vec2 lastOffset = {};
	int opCount = 25;
	Duration lastOperationTime = Duration::zero();

	DynamicTexture tex{ 1000, 1000 };

	HashTable<String, std::function<ColorF(double)>> colorTable;
	colorTable.emplace(U"HSV", [](double v) { return HSV{ v * 360 }; });
	colorTable.emplace(U"Red", [](double v) { return Math::Lerp(Palette::Black, Palette::Red, v); });
	colorTable.emplace(U"Lime", [](double v) { return Math::Lerp(Palette::Black, Palette::Lime, v); });

	decltype(colorTable)::iterator currentColor = colorTable.begin();

	const Font UIFont{ 32 };

	auto updateMandelbrot = [&]()
	{
		Stopwatch sw;
		sw.start();
		tex.fill(CreateMandelbrotImg(lastOffset, lastSize, tex.size(), opCount, currentColor->second));
		lastOperationTime = sw.elapsed();
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

		if (KeyZ.down()) { if (++currentColor == colorTable.end()) currentColor = colorTable.begin(); }

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
			pos.y = UIFont(U"[Z]: change color ({})"_fmt(currentColor->first)        ).draw(pos).bl().y;
			pos.y = UIFont(U"operation time: {}"_fmt(Format(lastOperationTime))      ).draw(pos).bl().y;
		}

	}
}
