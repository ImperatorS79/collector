

#include <string.h>
#include <iostream>

#include <SDL.h>
#include <SDL_ttf.h>

#include "collector.h"
#include "text.h"
#include "cli.h"



CLI::CLI()
{
	//create the initial empty tag field
	new_tag();
}


CLI::~CLI()
{
	destroy_tags();
}


void CLI::destroy_tags()
{
	for(unsigned int i = 0; i < tags.size(); i++)
	{
		delete tags[i];
	}
	
	tags.clear();
}


Text* CLI::current_tag()
{
	return tags[current];
}


void CLI::new_tag()
{
	Text* t = new Text("", config->get_cli_text_color());
	tags.push_back(t);

	current = tags.size() - 1;
}


void CLI::delete_tag()
{
	Text* t = current_tag();

	//ensure that there is always at least one tag preset
	if(tags.size() > 1)
	{
		delete t;
		tags.erase(tags.begin() + current);

		if(current > (tags.size() - 1))
		{
			current--;
		}
	}
	else
	{
		//if the user deleted the last tag, simply empty it
		t->set_text("");
	}
}


void CLI::handle_key(SDL_KeyboardEvent &e)
{
	switch(e.keysym.sym)
	{
		case SDLK_BACKSPACE:
		{
			std::string s = current_tag()->get_text();
			if(s.length() > 0)
			{
				s.pop_back();
				current_tag()->set_text(s);
			}
			break;
		}
		case SDLK_RETURN:
			destroy_tags();
			new_tag();
			break;
		case SDLK_ESCAPE:
			send_quit();
			break;
		case SDLK_DELETE:
			delete_tag();
			break;
		case SDLK_TAB:
			//autocomplete
			break;
		case SDLK_UP:
			break;
		case SDLK_DOWN:
			break;
		case SDLK_LEFT:
			if(current > 0)
				current--;
			break;
		case SDLK_RIGHT:
			if(current < (tags.size() - 1))
				current++;
			break;
		default:
			break;
	}
}


void CLI::handle_text(SDL_TextInputEvent &e)
{
	//space bar starts a new tag
	if(e.text[0] == ' ')
	{
		//if the last tag is empty, skip to it (rather than adding another)
		if(tags[tags.size() - 1]->get_text().length() == 0)
		{
			current = tags.size() - 1;
		}
		else
		{
			new_tag();
		}
	}
	else
	{
		Text* t = current_tag();
		t->set_text(t->get_text() += e.text);
	}
}


void CLI::render()
{
	int win_w = 0;
	int win_h = 0;
	SDL_GetWindowSize(window, &win_w, &win_h);

	SDL_Rect current_rect = {0, win_h - CLI_height, win_w, CLI_height};

	setRenderDrawColor(renderer, config->get_highlight_color());

	//draw each tags text
	int x = CLI_padding;
	for(unsigned int i = 0; i < tags.size(); i++)
	{
		Text* t = tags[i];
		
		if(i == current)
		{
			current_rect.x = x - CLI_padding;
			current_rect.w = t->width() + (CLI_padding * 2);
			SDL_RenderFillRect(renderer, &current_rect);
		}

		t->render(x, win_h - 16);
		x += t->width() + (CLI_padding * 2);
	}
}


void CLI::send_quit()
{
	SDL_Event e;
	e.type = SDL_QUIT;

	//SDL copies memory into event queue, so this is memory safe
	SDL_PushEvent(&e);
}
