# Software Architecture

## Components

```{arch} <Overall Software Component Architecture>
:id: SWARCH_001
:realizes: REQ_2, REQ_36, REQ_38, REQ_39, REQ_40, REQ_42, REQ_43, REQ_44, REQ_46, REQ_47, REQ_48, REQ_50, REQ_51, REQ_53, REQ_54, REQ_55, REQ_56, REQ_59
```

{% set selected = build_config.components_info | default([]) | map(attribute='name') | list %}
{% set spled_brightness = 'components_spled' in selected and 'components_brightness_controller' in selected and config.BRIGHTNESS_ADJUSTMENT_ENABLED %}
{% set power_button_calls = ['IsKeyPressed', 'SetPowerKeyPressedEvent'] %}
{% set power_signal_processing_calls = ['GetPowerKeyPressedEvent', 'GetPowerState', 'SetPowerState'] %}
{% set main_control_knob_calls = ['GetMainKnobValue', 'IsKeyPressed', 'SetMainKnobValue'] %}
{% set brightness_controller_calls = ['SetBrightnessValue'] %}
{%- if config.BRIGHTNESS_ADJUSTMENT_AUTOMATIC %}
{%- set brightness_controller_calls = brightness_controller_calls + ['SetBrightnessAdjustmentCounter'] %}
{%- else %}
{%- set brightness_controller_calls = brightness_controller_calls + ['GetMainKnobValue'] %}
{%- endif %}
{% set light_controller_calls = ['GetPowerState', 'SetLightValue'] %}
{%- if config.BLINKING %}
{%- set light_controller_calls = light_controller_calls + ['GetMainKnobValue'] %}
{%- endif %}
{%- if config.BRIGHTNESS_ADJUSTMENT_ENABLED %}
{%- set light_controller_calls = light_controller_calls + ['GetBrightnessValue'] %}
{%- endif %}
{%- if config.BRIGHTNESS_ADJUSTMENT_PERIOD %}
{%- set light_controller_calls = light_controller_calls + ['GetBrightnessAdjustmentCounter'] %}
{%- endif %}
{% set auto_off_calls = ['GetPowerKeyPressedEvent', 'GetPowerState', 'IsKeyPressed', 'SetAutoOffState', 'SetPowerState'] %}
{% set console_interface_calls = ['GetLightValue'] %}
{% set rte_calls = [] %}
{%- if 'components_power_button' in selected %}
{%- set rte_calls = rte_calls + power_button_calls %}
{%- endif %}
{%- if 'components_power_signal_processing' in selected %}
{%- set rte_calls = rte_calls + power_signal_processing_calls %}
{%- endif %}
{%- if 'components_main_control_knob' in selected %}
{%- set rte_calls = rte_calls + main_control_knob_calls %}
{%- endif %}
{%- if 'components_brightness_controller' in selected %}
{%- set rte_calls = rte_calls + brightness_controller_calls %}
{%- endif %}
{%- if 'components_light_controller' in selected %}
{%- set rte_calls = rte_calls + light_controller_calls %}
{%- endif %}
{%- if 'components_auto_off' in selected %}
{%- set rte_calls = rte_calls + auto_off_calls %}
{%- endif %}
{%- if 'components_console_interface' in selected %}
{%- set rte_calls = rte_calls + console_interface_calls %}
{%- endif %}
```{mermaid}
:zoom:

classDiagram
    direction TB
{% if 'components_main' in selected %}
    class main["Main"] {
        <<component>>
    }
    main ..> os : Run
{% endif %}
{% if 'components_os' in selected %}
    class os["OS"] {
        <<component>>
{%- if 'components_main' in selected %}
        +Run()
{%- endif %}
    }
    os ..> spled : spled
{% endif %}
{% if 'components_spled' in selected %}
    class spled["SPLED"] {
        <<component>>
{%- if 'components_os' in selected %}
        +spled()
{%- endif %}
    }
    spled ..> power_button : powerButton
    spled ..> power_signal_processing : powerSignalProcessing
    spled ..> main_control_knob : mainControlKnob
{% if spled_brightness %}
    spled ..> brightness_controller : brightnessController
{% endif %}
{% if 'components_auto_off' in selected %}
    spled ..> auto_off : autoOff
{% endif %}
    spled ..> light_controller : lightController
    spled ..> console_interface : consoleInterface
{% endif %}
{% if 'components_power_button' in selected %}
    class power_button["PowerButton"] {
        <<component>>
{%- if 'components_spled' in selected %}
        +powerButton()
{%- endif %}
    }
    power_button ..> rte : {{ power_button_calls | sort | join(', ') }}
{% endif %}
{% if 'components_power_signal_processing' in selected %}
    class power_signal_processing["PowerSignalProcessing"] {
        <<component>>
{%- if 'components_spled' in selected %}
        +powerSignalProcessing()
{%- endif %}
    }
    power_signal_processing ..> rte : {{ power_signal_processing_calls | sort | join(', ') }}
{% endif %}
{% if 'components_main_control_knob' in selected %}
    class main_control_knob["MainControlKnob"] {
        <<component>>
{%- if 'components_spled' in selected %}
        +mainControlKnob()
{%- endif %}
    }
    main_control_knob ..> rte : {{ main_control_knob_calls | sort | join(', ') }}
{% endif %}
{% if 'components_brightness_controller' in selected %}
    class brightness_controller["BrightnessController"] {
        <<component>>
{%- if spled_brightness %}
        +brightnessController()
{%- endif %}
    }
    brightness_controller ..> rte : {{ brightness_controller_calls | sort | join(', ') }}
{% endif %}
{% if 'components_light_controller' in selected %}
    class light_controller["LightController"] {
        <<component>>
{%- if 'components_spled' in selected %}
        +lightController()
{%- endif %}
    }
    light_controller ..> rte : {{ light_controller_calls | sort | join(', ') }}
{% endif %}
{% if 'components_auto_off' in selected %}
    class auto_off["AutoOff"] {
        <<component>>
{%- if 'components_spled' in selected %}
        +autoOff()
{%- endif %}
    }
    auto_off ..> rte : {{ auto_off_calls | sort | join(', ') }}
{% endif %}
{% if 'components_console_interface' in selected %}
    class console_interface["ConsoleInterface"] {
        <<component>>
{%- if 'components_spled' in selected %}
        +consoleInterface()
{%- endif %}
    }
    console_interface ..> rte : {{ console_interface_calls | sort | join(', ') }}
{% endif %}
{% if 'components_keyboard_interface' in selected %}
    class keyboard_interface["KeyboardInterface"] {
        <<component>>
{%- if 'components_rte' in selected %}
        +IsKeyPressed()
{%- endif %}
    }
{% endif %}
{% if 'components_rte' in selected %}
{% if 'components_keyboard_interface' in selected %}
    rte ..> keyboard_interface : IsKeyPressed
{% endif %}
    class rte["RTE"] {
        <<RTE>>
{%- for call in rte_calls | unique | sort %}
        +{{ call }}()
{%- endfor %}
    }
{% endif %}
```
