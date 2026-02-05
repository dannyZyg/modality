enum class ParamWidgetType
{
    slider,
    toggle
};

struct Param
{
    ParamWidgetType type;
    double value;
    double min;
    double max;
};
