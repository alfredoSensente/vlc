/*****************************************************************************
 * preferences_widgets.cpp : wxWindows plugin for vlc
 *****************************************************************************
 * Copyright (C) 2000-2001 VideoLAN
 * $Id: preferences_widgets.cpp,v 1.9 2003/11/02 22:16:32 gbazin Exp $
 *
 * Authors: Gildas Bazin <gbazin@netcourrier.com>
 *          Sigmund Augdal <sigmunau@idi.ntnu.no>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111, USA.
 *****************************************************************************/

/*****************************************************************************
 * Preamble
 *****************************************************************************/
#include <stdlib.h>                                      /* malloc(), free() */
#include <errno.h>                                                 /* ENOMEM */
#include <string.h>                                            /* strerror() */
#include <stdio.h>

#include <vlc/vlc.h>
#include <vlc/intf.h>

#include <vlc_help.h>

#include "wxwindows.h"
#include "preferences_widgets.h"

/*****************************************************************************
 * CreateConfigControl wrapper
 *****************************************************************************/
ConfigControl *CreateConfigControl( vlc_object_t *p_this,
                                    module_config_t *p_item, wxWindow *parent )
{
    ConfigControl *p_control = NULL;

    switch( p_item->i_type )
    {
    case CONFIG_ITEM_MODULE:
        p_control = new ModuleConfigControl( p_this, p_item, parent );
        break;

    case CONFIG_ITEM_STRING:
        if( !p_item->ppsz_list )
        {
            p_control = new StringConfigControl( p_item, parent );
        }
        else
        {
            p_control = new StringListConfigControl( p_item, parent );
        }
        break;

    case CONFIG_ITEM_FILE:
    case CONFIG_ITEM_DIRECTORY:
        p_control = new FileConfigControl( p_item, parent );
        break;

    case CONFIG_ITEM_INTEGER:
        if( p_item->i_min != 0 || p_item->i_max != 0 )
        {
            p_control = new RangedIntConfigControl( p_item, parent );
        }
        else
        {
            p_control = new IntegerConfigControl( p_item, parent );
        }
        break;

    case CONFIG_ITEM_KEY:
        p_control = new KeyConfigControl( p_item, parent );
        break;

    case CONFIG_ITEM_FLOAT:
        p_control = new FloatConfigControl( p_item, parent );
        break;

    case CONFIG_ITEM_BOOL:
        p_control = new BoolConfigControl( p_item, parent );
        break;

    default:
        break;
    }

    return p_control;
}

/*****************************************************************************
 * ConfigControl implementation
 *****************************************************************************/
ConfigControl::ConfigControl( module_config_t *p_item, wxWindow *parent )
  : wxPanel( parent ), name( wxU(p_item->psz_name) ),
    i_type( p_item->i_type ), b_advanced( p_item->b_advanced )
{
    sizer = new wxBoxSizer( wxHORIZONTAL );
}

ConfigControl::~ConfigControl()
{
}

wxSizer *ConfigControl::Sizer()
{
    return sizer;
}

wxString ConfigControl::GetName()
{
    return name;
}

int ConfigControl::GetType()
{
    return i_type;
}

vlc_bool_t ConfigControl::IsAdvanced()
{
    return b_advanced;
}

/*****************************************************************************
 * KeyConfigControl implementation
 *****************************************************************************/
static wxString KeysList[] =
{
    wxT("Unset"),
    wxT("Left"),
    wxT("Right"),
    wxT("Up"),
    wxT("Down"),
    wxT("Space"),
    wxT("Enter"),
    wxT("F1"),
    wxT("F2"),
    wxT("F3"),
    wxT("F4"),
    wxT("F5"),
    wxT("F6"),
    wxT("F7"),
    wxT("F8"),
    wxT("F9"),
    wxT("F10"),
    wxT("F11"),
    wxT("F12"),
    wxT("Home"),
    wxT("End"),
    wxT("Menu"),
    wxT("Esc"),
    wxT("Page Up"),
    wxT("Page Down"),
    wxT("Tab"),
    wxT("Backspace"),
    wxT("a"),
    wxT("b"),
    wxT("c"),
    wxT("d"),
    wxT("e"),
    wxT("f"),
    wxT("g"),
    wxT("h"),
    wxT("i"),
    wxT("j"),
    wxT("k"),
    wxT("l"),
    wxT("m"),
    wxT("n"),
    wxT("o"),
    wxT("p"),
    wxT("q"),
    wxT("r"),
    wxT("s"),
    wxT("t"),
    wxT("u"),
    wxT("v"),
    wxT("w"),
    wxT("x"),
    wxT("y"),
    wxT("z"),
    wxT("+"),
    wxT("="),
    wxT("-"),
    wxT(","),
    wxT("."),
    wxT("<"),
    wxT(">"),
    wxT("`"),
    wxT("/"),
    wxT(";"),
    wxT("'"),
    wxT("\\"),
    wxT("["),
    wxT("]"),
    wxT("*")
};

KeyConfigControl::KeyConfigControl( module_config_t *p_item, wxWindow *parent )
  : ConfigControl( p_item, parent )
{
    label = new wxStaticText(this, -1, wxU(p_item->psz_text));
    alt = new wxCheckBox( this, -1, wxU(_("Alt")) );
    alt->SetValue( p_item->i_value & KEY_MODIFIER_ALT );
    ctrl = new wxCheckBox( this, -1, wxU(_("Ctrl")) );
    ctrl->SetValue( p_item->i_value & KEY_MODIFIER_CTRL );
    shift = new wxCheckBox( this, -1, wxU(_("Shift")) );
    shift->SetValue( p_item->i_value & KEY_MODIFIER_SHIFT );
    combo = new wxComboBox( this, -1, wxT(""), wxDefaultPosition,
                            wxDefaultSize, WXSIZEOF(KeysList), KeysList,
                            wxCB_READONLY );
    for( unsigned int i = 0; i < WXSIZEOF(KeysList); i++ )
    {
        combo->SetClientData( i, (void*)keys[i].i_key_code );
        if( keys[i].i_key_code ==
            ( ((unsigned int)p_item->i_value) & ~KEY_MODIFIER ) )
        {
            combo->SetSelection( i );
            combo->SetValue( wxU(_(keys[i].psz_key_string)) );
        }
    }

    sizer->Add( label, 2, wxALIGN_CENTER_VERTICAL | wxALL | wxEXPAND, 5 );
    sizer->Add( alt,   1, wxALIGN_CENTER_VERTICAL | wxALL | wxEXPAND, 5 );
    sizer->Add( ctrl,  1, wxALIGN_CENTER_VERTICAL | wxALL | wxEXPAND, 5 );
    sizer->Add( shift, 1, wxALIGN_CENTER_VERTICAL | wxALL | wxEXPAND, 5 );
    sizer->Add( combo, 2, wxALIGN_CENTER_VERTICAL | wxALL | wxEXPAND, 5 );
    sizer->Layout();
    this->SetSizerAndFit( sizer );
}

KeyConfigControl::~KeyConfigControl()
{
    ;
}

int KeyConfigControl::GetIntValue()
{
    int result = 0;
    if( alt->IsChecked() )
    {
        result |= KEY_MODIFIER_ALT;
    }
    if( ctrl->IsChecked() )
    {
        result |= KEY_MODIFIER_CTRL;
    }
    if( shift->IsChecked() )
    {
        result |= KEY_MODIFIER_SHIFT;
    }
    int selected = combo->GetSelection();
    if( selected != -1 )
    {
        result |= (int)combo->GetClientData( selected );
    }
    return result;
}

/*****************************************************************************
 * ModuleConfigControl implementation
 *****************************************************************************/
ModuleConfigControl::ModuleConfigControl( vlc_object_t *p_this,
                                          module_config_t *p_item,
                                          wxWindow *parent )
  : ConfigControl( p_item, parent )
{
    vlc_list_t *p_list;
    module_t *p_parser;

    label = new wxStaticText(this, -1, wxU(p_item->psz_text));
    combo = new wxComboBox( this, -1, wxU(p_item->psz_value),
                            wxDefaultPosition, wxDefaultSize,
                            0, NULL, wxCB_READONLY | wxCB_SORT );

    /* build a list of available modules */
    p_list = vlc_list_find( p_this, VLC_OBJECT_MODULE, FIND_ANYWHERE );
    combo->Append( wxU(_("Default")), (void *)NULL );
    combo->SetSelection( 0 );
    for( int i_index = 0; i_index < p_list->i_count; i_index++ )
    {
        p_parser = (module_t *)p_list->p_values[i_index].p_object ;

        if( !strcmp( p_parser->psz_capability, p_item->psz_type ) )
        {
            combo->Append( wxU(p_parser->psz_longname),
                           p_parser->psz_object_name );
            if( p_item->psz_value && !strcmp(p_item->psz_value, 
                                             p_parser->psz_object_name) )
                combo->SetValue( wxU(p_parser->psz_longname) );
        }
    }
    vlc_list_release( p_list );

    combo->SetToolTip( wxU(p_item->psz_longtext) );
    sizer->Add( label, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5 );
    sizer->Add( combo, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5 );
    sizer->Layout();
    this->SetSizerAndFit( sizer );
}

ModuleConfigControl::~ModuleConfigControl()
{
    ;
}

wxString ModuleConfigControl::GetPszValue()
{
    return wxU( (char *)combo->GetClientData( combo->GetSelection() ));
}

/*****************************************************************************
 * StringConfigControl implementation
 *****************************************************************************/
StringConfigControl::StringConfigControl( module_config_t *p_item,
                                          wxWindow *parent )
  : ConfigControl( p_item, parent )
{
    label = new wxStaticText(this, -1, wxU(p_item->psz_text));
    sizer->Add( label, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5 );
    textctrl = new wxTextCtrl( this, -1, 
                               wxU(p_item->psz_value),
                               wxDefaultPosition,
                               wxDefaultSize,
                               wxTE_PROCESS_ENTER);
    textctrl->SetToolTip( wxU(p_item->psz_longtext) );
    sizer->Add( textctrl, 1, wxALL, 5 );
    sizer->Layout();
    this->SetSizerAndFit( sizer );
}

StringConfigControl::~StringConfigControl()
{
    ;
}

wxString StringConfigControl::GetPszValue()
{
    return textctrl->GetValue();
}

/*****************************************************************************
 * StringListConfigControl implementation
 *****************************************************************************/
StringListConfigControl::StringListConfigControl( module_config_t *p_item,
                                                  wxWindow *parent )
  : ConfigControl( p_item, parent )
{
    label = new wxStaticText(this, -1, wxU(p_item->psz_text));
    sizer->Add( label, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5 );
    combo = new wxComboBox( this, -1, wxU(p_item->psz_value),
                            wxDefaultPosition, wxDefaultSize,
                            0, NULL, wxCB_READONLY );

    /* build a list of available options */
    for( int i_index = 0; p_item->ppsz_list[i_index];
         i_index++ )
    {
        combo->Append( wxU(p_item->ppsz_list[i_index]) );
        if( p_item->psz_value && !strcmp( p_item->psz_value,
                                          p_item->ppsz_list[i_index] ) )
            combo->SetSelection( i_index );
    }

    if( p_item->psz_value )
        combo->SetValue( wxU(p_item->psz_value) );
    combo->SetToolTip( wxU(p_item->psz_longtext) );
    sizer->Add( combo, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5 );    
    sizer->Layout();
    this->SetSizerAndFit( sizer );
}

StringListConfigControl::~StringListConfigControl()
{
    ;
}

wxString StringListConfigControl::GetPszValue()
{
    return combo->GetStringSelection();
}

/*****************************************************************************
 * FileConfigControl implementation
 *****************************************************************************/
FileConfigControl::FileConfigControl( module_config_t *p_item,
                                      wxWindow *parent )
  : ConfigControl( p_item, parent )
{
    directory = p_item->i_type == CONFIG_ITEM_DIRECTORY;
    label = new wxStaticText(this, -1, wxU(p_item->psz_text));
    sizer->Add( label, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5 );
    textctrl = new wxTextCtrl( this, -1, 
                               wxU(p_item->psz_value),
                               wxDefaultPosition,
                               wxDefaultSize,
                               wxTE_PROCESS_ENTER);
    textctrl->SetToolTip( wxU(p_item->psz_longtext) );
    sizer->Add( textctrl, 1, wxALL, 5 );
    browse = new wxButton( this, wxID_HIGHEST, wxU(_("Browse...")) );
    sizer->Add( browse, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
    sizer->Layout();
    this->SetSizerAndFit( sizer );
}

BEGIN_EVENT_TABLE(FileConfigControl, wxPanel)
    /* Button events */
    EVT_BUTTON(wxID_HIGHEST, FileConfigControl::OnBrowse)
END_EVENT_TABLE()

void FileConfigControl::OnBrowse( wxCommandEvent& event )
{
    if( directory )
    {
        wxDirDialog dialog( this, wxU(_("Choose Directory")) );

        if( dialog.ShowModal() == wxID_OK )
        {
            textctrl->SetValue( dialog.GetPath() );      
        }
    }
    else
    {
        wxFileDialog dialog( this, wxU(_("Choose File")),
                             wxT(""), wxT(""), wxT("*.*"),
#if defined( __WXMSW__ )
                             wxOPEN
#else
                             wxOPEN | wxSAVE
#endif
                           );
    }
}

FileConfigControl::~FileConfigControl()
{
    ;
}
    
wxString FileConfigControl::GetPszValue()
{
    return textctrl->GetValue();
}

/*****************************************************************************
 * IntegerConfigControl implementation
 *****************************************************************************/
IntegerConfigControl::IntegerConfigControl( module_config_t *p_item,
                                            wxWindow *parent )
  : ConfigControl( p_item, parent )
{
    label = new wxStaticText(this, -1, wxU(p_item->psz_text));
    spin = new wxSpinCtrl( this, -1,
                           wxString::Format(wxT("%d"),
                                            p_item->i_value),
                           wxDefaultPosition, wxDefaultSize,
                           wxSP_ARROW_KEYS,
                           -16000, 16000, p_item->i_value);
    spin->SetToolTip( wxU(p_item->psz_longtext) );
    sizer->Add( label, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5 );
    sizer->Add( spin, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );    
    sizer->Layout();
    this->SetSizerAndFit( sizer );
}

IntegerConfigControl::~IntegerConfigControl()
{
    ;
}

int IntegerConfigControl::GetIntValue()
{
    return spin->GetValue();
}

/*****************************************************************************
 * RangedIntConfigControl implementation
 *****************************************************************************/
RangedIntConfigControl::RangedIntConfigControl( module_config_t *p_item,
                                                wxWindow *parent )
  : ConfigControl( p_item, parent )
{
    label = new wxStaticText(this, -1, wxU(p_item->psz_text));
    slider = new wxSlider( this, -1, p_item->i_value, p_item->i_min,
                           p_item->i_max, wxDefaultPosition, wxDefaultSize,
                           wxSL_LABELS | wxSL_HORIZONTAL );
    slider->SetToolTip( wxU(p_item->psz_longtext) );
    sizer->Add( label, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5 );
    sizer->Add( slider, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5 );    
    sizer->Layout();
    this->SetSizerAndFit( sizer );
}

RangedIntConfigControl::~RangedIntConfigControl()
{
    ;
}

int RangedIntConfigControl::GetIntValue()
{
    return slider->GetValue();
}

/*****************************************************************************
 * FloatConfigControl implementation
 *****************************************************************************/
FloatConfigControl::FloatConfigControl( module_config_t *p_item,
                                        wxWindow *parent )
  : ConfigControl( p_item, parent )
{
    label = new wxStaticText(this, -1, wxU(p_item->psz_text));
    textctrl = new wxTextCtrl( this, -1,
                               wxString::Format(wxT("%f"),
                                                p_item->f_value),
                               wxDefaultPosition, wxDefaultSize,
                               wxTE_PROCESS_ENTER );
    textctrl->SetToolTip( wxU(p_item->psz_longtext) );
    sizer->Add( label, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5 );
    sizer->Add( textctrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
    sizer->Layout();
    this->SetSizerAndFit( sizer );
}

FloatConfigControl::~FloatConfigControl()
{
    ;
}

float FloatConfigControl::GetFloatValue()
{
    float f_value;
    if( (wxSscanf(textctrl->GetValue(), wxT("%f"), &f_value) == 1) )
        return f_value;
    else return 0.0;
}

/*****************************************************************************
 * BoolConfigControl implementation
 *****************************************************************************/
BoolConfigControl::BoolConfigControl( module_config_t *p_item,
                                      wxWindow *parent )
  : ConfigControl( p_item, parent )
{
    checkbox = new wxCheckBox( this, -1, wxU(p_item->psz_text) );
    if( p_item->i_value ) checkbox->SetValue(TRUE);
    checkbox->SetToolTip( wxU(p_item->psz_longtext) );
    sizer->Add( checkbox, 0, wxALL, 5 );
    sizer->Layout();
    this->SetSizerAndFit( sizer );
}

BoolConfigControl::~BoolConfigControl()
{
    ;
}

int BoolConfigControl::GetIntValue()
{
    if( checkbox->IsChecked() )
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
