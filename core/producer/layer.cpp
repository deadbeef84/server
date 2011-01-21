#include "../stdafx.h"

#include "layer.h"

#include "../mixer/frame/draw_frame.h"
#include "../mixer/image/image_mixer.h"
#include "../mixer/audio/audio_mixer.h"
#include "../producer/frame_producer.h"

#include "../video_format.h"

#include <common/utility/assert.h>

namespace caspar { namespace core {

struct layer::implementation : boost::noncopyable
{					
	safe_ptr<frame_producer>	foreground_;
	safe_ptr<frame_producer>	background_;
	safe_ptr<draw_frame>		last_frame_;
	bool						is_paused_;

public:
	implementation() 
		: foreground_(frame_producer::empty())
		, background_(frame_producer::empty())
		, last_frame_(draw_frame::empty())
		, is_paused_(false){}
	
	void load(const safe_ptr<frame_producer>& frame_producer, bool play_on_load)
	{			
		background_ = frame_producer;
		if(play_on_load)
			play();			
	}

	void preview(const safe_ptr<frame_producer>& frame_producer)
	{
		stop();
		load(frame_producer, false);		
		try
		{
			last_frame_ = frame_producer->receive();
		}
		catch(...)
		{
			CASPAR_LOG_CURRENT_EXCEPTION();
			clear();
		}
	}
	
	void play()
	{			
		if(is_paused_)			
			is_paused_ = false;
		else
		{
			background_->set_leading_producer(foreground_);
			foreground_ = background_;
			background_ = frame_producer::empty();
		}
	}

	void pause()
	{
		is_paused_ = true;
	}

	void stop()
	{
		pause();
		last_frame_ = draw_frame::empty();
		foreground_ = frame_producer::empty();
	}

	void clear()
	{
		stop();
		background_ = frame_producer::empty();
	}
	
	safe_ptr<draw_frame> receive()
	{		
		if(is_paused_)
			return last_frame_;

		try
		{
			last_frame_ = foreground_->receive(); 
			if(last_frame_ == draw_frame::eof())
			{
				CASPAR_ASSERT(foreground_ != frame_producer::empty());

				auto following = foreground_->get_following_producer();
				following->set_leading_producer(foreground_);
				foreground_ = following;

				last_frame_ = receive();
			}
		}
		catch(...)
		{
			CASPAR_LOG_CURRENT_EXCEPTION();
			stop();
		}

		return last_frame_;
	}
};

layer::layer() 
{
	impl_ = new implementation();
}
layer::layer(layer&& other) : impl_(std::move(other.impl_)){other.impl_ = nullptr;}
layer::~layer()
{
	delete impl_.fetch_and_store(nullptr);
}
layer& layer::operator=(layer&& other)
{
	impl_ = std::move(other.impl_);	
	other.impl_ = nullptr;
	return *this;
}
void layer::swap(layer& other)
{
	impl_ = other.impl_.compare_and_swap(impl_, other.impl_);
}
void layer::load(const safe_ptr<frame_producer>& frame_producer, bool play_on_load){return impl_->load(frame_producer, play_on_load);}	
void layer::preview(const safe_ptr<frame_producer>& frame_producer){return impl_->preview(frame_producer);}	
void layer::play(){impl_->play();}
void layer::pause(){impl_->pause();}
void layer::stop(){impl_->stop();}
void layer::clear(){impl_->clear();}
safe_ptr<draw_frame> layer::receive() {return impl_->receive();}
safe_ptr<frame_producer> layer::foreground() const { return impl_->foreground_;}
safe_ptr<frame_producer> layer::background() const { return impl_->background_;}
}}