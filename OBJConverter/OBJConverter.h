#pragma once

namespace Boolka
{

    class OBJConverter
    {
    public:
        OBJConverter();
        ~OBJConverter();

        bool Load(std::string filename);
        bool Save(std::string filename);

    private:
        tinyobj::attrib_t m_attrib;
        std::vector<tinyobj::shape_t> m_shapes;
        std::vector<tinyobj::material_t> m_materials;

        std::string m_warn;
        std::string m_err;
    };

}
