/**
 * @file screen_menu_tools.hpp
 * @brief This is temporary menu enabling dock position and tool offset view and edit. Simple manual calibration of the dock position is included.
 */

#pragma once

#include "screen_menu.hpp"
#include "MItem_tools.hpp"
#include "selftest_frame.hpp"
#include "MItem_hardware.hpp"
#include "WindowItemTempLabel.hpp"

class MI_PICKUP_TOOL : public IWindowMenuItem {
    static constexpr const char *label = N_("Pickup Tool");

public:
    MI_PICKUP_TOOL();

    virtual void click(IWindowMenu &window_menu) override;
};

namespace detail {
using ScreenMenuToolSetup = ScreenMenu<GuiDefaults::MenuFooter, MI_RETURN, MI_PICKUP_TOOL>;
}

class ScreenMenuToolSetup : public detail::ScreenMenuToolSetup {
public:
    ScreenMenuToolSetup();

    constexpr static const char *labels[] = { N_("TOOL 1"), N_("TOOL 2"), N_("TOOL 3"), N_("TOOL 4"), N_("TOOL 5") };
};

class I_MI_TOOL : public IWindowMenuItem {

public:
    I_MI_TOOL(uint8_t tool_index);

protected:
    void click(IWindowMenu &window_menu);

private:
    const uint8_t tool_index;
};

template <int N>
class MI_TOOL : public I_MI_TOOL {
public:
    MI_TOOL()
        : I_MI_TOOL(N) {}
};

class MI_PARK_TOOL : public IWindowMenuItem {
    static constexpr const char *label = N_("Park Current Tool");

public:
    MI_PARK_TOOL();

    virtual void click(IWindowMenu &window_menu) override;
};

namespace detail {
using ScreenMenuTools = ScreenMenu<GuiDefaults::MenuFooter, MI_RETURN, MI_TOOL<0>, MI_TOOL<1>, MI_TOOL<2>, MI_TOOL<3>, MI_TOOL<4>, MI_PARK_TOOL>;
}

class ScreenMenuTools : public detail::ScreenMenuTools {
public:
    constexpr static const char *label = N_("TOOLS");
    ScreenMenuTools();
};

class MI_INFO_DWARF_BOARD_TEMPERATURE : public WI_TEMP_LABEL_t {
    static constexpr const char *const label = N_("Dwarf Board Temp");

public:
    MI_INFO_DWARF_BOARD_TEMPERATURE();
};

class MI_INFO_DWARF_MCU_TEMPERATURE : public WI_TEMP_LABEL_t {
    static constexpr const char *const label = N_("Dwarf MCU Temp");

public:
    MI_INFO_DWARF_MCU_TEMPERATURE();
};

class I_MI_INFO_HEATBREAK_N_TEMP : public WI_TEMP_LABEL_t {
    static constexpr const char *const generic_label = N_("Heatbreak Temp"); // Generic string for single tool version

public:
    I_MI_INFO_HEATBREAK_N_TEMP(const char *const specific_label, int index);
};

template <int N>
class MI_INFO_HEATBREAK_N_TEMP : public I_MI_INFO_HEATBREAK_N_TEMP {
    static_assert(N >= 0 && N <= 4, "bad input");
    static consteval const char *get_name() {
        switch (N) {
        case 0:
            return N_("Heatbreak 1 temp");
        case 1:
            return N_("Heatbreak 2 temp");
        case 2:
            return N_("Heatbreak 3 temp");
        case 3:
            return N_("Heatbreak 4 temp");
        case 4:
            return N_("Heatbreak 5 temp");
        }
        consteval_assert_false();
        return "";
    }

    static constexpr const char *const specific_label = get_name();

public:
    MI_INFO_HEATBREAK_N_TEMP()
        : I_MI_INFO_HEATBREAK_N_TEMP(specific_label, N) {
    }
};

class I_MI_INFO_NOZZLE_N_TEMP : public WI_TEMP_LABEL_t {
    static constexpr const char *const generic_label = N_("Nozzle Temperature"); // Generic string for single tool version

public:
    I_MI_INFO_NOZZLE_N_TEMP(const char *const specific_label, int index);
};

template <int N>
class MI_INFO_NOZZLE_N_TEMP : public I_MI_INFO_NOZZLE_N_TEMP {
    static_assert(N >= 0 && N <= 4, "bad input");
    static consteval const char *get_name() {
        switch (N) {
        case 0:
            return N_("Nozzle 1 Temperature");
        case 1:
            return N_("Nozzle 2 Temperature");
        case 2:
            return N_("Nozzle 3 Temperature");
        case 3:
            return N_("Nozzle 4 Temperature");
        case 4:
            return N_("Nozzle 5 Temperature");
        }
        consteval_assert_false();
        return "";
    }

    static constexpr const char *const specific_label = get_name();

public:
    MI_INFO_NOZZLE_N_TEMP()
        : I_MI_INFO_NOZZLE_N_TEMP(specific_label, N) {
    }
};

/**
 * @brief Tool-specific odometer item.
 * @param OdometerT class with constructor that takes N and label.
 * @param N which extruder [indexed from 0]
 */
template <class OdometerT, int N>
class MI_ODOMETER_N : public OdometerT {
    static_assert(N >= 0 && N <= 4, "bad input");
    static consteval const char *get_name() {
        switch (N) {
        case 0:
            return N_("  Tool 1"); // Keep space in front for menu alignment
        case 1:
            return N_("  Tool 2");
        case 2:
            return N_("  Tool 3");
        case 3:
            return N_("  Tool 4");
        case 4:
            return N_("  Tool 5");
        }
        consteval_assert_false();
        return "";
    }

    static constexpr const char *const specific_label = get_name();

public:
    MI_ODOMETER_N()
        : OdometerT(specific_label, N) {
    }
};

// Specializations of odometer display for particular tool
template <int N>
using MI_ODOMETER_DIST_E_N = MI_ODOMETER_N<MI_ODOMETER_DIST_E, N>;
template <int N>
using MI_ODOMETER_TOOL_N = MI_ODOMETER_N<MI_ODOMETER_TOOL, N>;
