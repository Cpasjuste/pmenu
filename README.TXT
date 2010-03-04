pmenu_i386_20091209:
- Added : settings for "Media" section in the skin configuration file (gui.cfg)
- Added : "applications_title_description_y" setting in gui.cfg, to set where the text start withing the application_box
- Fixed : the small font in the application list does not honor the highlight color when selected.
- Fixed : the media list runs off the right side - it does not honor the width specified for the application list.
- Fixed : Partially fixed fonts ( except for the scrolling one )
- Fixed : The skin preview image does not honor the maximum width for the preview area.
- Fixed : The confirm box's confirm_box_x and confirm_box_y attributes don't appear to affect anything.
- Fixed : The dialog says "to your favorites applications" instead of "to your favourite applications"
- Fixed : The scrolling text under applications has a visual glitch when glyphs leave to the left: they disappear one glyph at a time instead of linearly fading away.

pmenu_i386_20091127_r2:
- Added Text Aliasing option to the skin configuration file (gui.cfg)
- Added Text Bold Italic Normal option to the skin configuration file (gui.cfg)

pmenu_i386_20091127:
- Removed device TOTAL space info text.
- Partially fixed text charachter spacing (seems better, some work is still needed)
- Added 3 diffirent background ( emus/games/misc/favorites, media, settings )
- Fixed a bug with the text in the applications box, was not respecting application_box_width
- Fixed another bug in the applications box, application_box_w was not correctly handled

pmenu_i386_20091126:
- Added settings text interlines spacing option to the skin configuration file (gui.cfg)
- Added proper x/y coordinates for applications_box.
- Added proper x/y coordinates for the setting menu text (same as applications_box)
- Added "applications_box_w" setting to the skin configuration file (gui.cfg)
- Added "icon_scale_max" and "icon_scale_min" to the skin configuration file (gui.cfg)
- Added "applications_spacing" for text interlines spacing to the skin configuration file (gui.cfg)
- Changed "applications" section by "Media" section
- Changed "divers" section by "Misc" section
- Added a specific font for each infos settings, and with custom color and size.
- Added an "hightlight" category icon option in the skin configuration file (gui.cfg) so you can set a different icon when selected.


pmenu_i386_20091122 :
- Added infos text color parameter to the skin configuration file (gui.cfg)
- Added skin data path to the skin configuration file (gui.cfg)

pmenu_i386_20091121_r3 :
- Added cpu/sd1/sd2/clock icon/text x/y option to the skin configuration file (gui.cfg)
- Fixed keyboard autorepeat
- Added a delay for description scrolling
- Added a "preview_pic_w" option to the skin configuration file (gui.cfg), to set the
  preview pic maximum width ( height will be a 1.6666 ratio of that to preserve
  the pandora screen ration (800/480) ).

pmenu_i386_20091121_r2 :
- Fixed : skin configuration file (gui.cfg) was not read while switching skins
- Removed the exit on mouse click function

pmenu_i386_20091121 :
- Added linux i386 makefile
- Added fonts size/color configuration to skins configuration file (gui.cfg)
- Added Cpu speed selector to settings menu (pandora only)
- Added Skins data verification routine to prevent loading a skin if some file are missing
