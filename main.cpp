#include <iostream>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <memory>
#include "ftxui/dom/node.hpp"

using namespace ftxui;

int main(void){
	auto document = vbox({
			text("TX AUDIO PLAYER") | borderHeavy
			});

	auto screen = Screen::Create(
			Dimension::Full(), // set full screen width
			Dimension::Full() //set full screen height
	);
	Render(screen,document);
	screen.Print();
	std::cout<<std::endl;
	return EXIT_SUCCESS;
}
