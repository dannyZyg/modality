enum class ParamWidgetType
{
    slider,
    rangeSlider,
    toggle
};

struct Param
{
    ParamWidgetType type;
    double value;
    double min;
    double max;
};
