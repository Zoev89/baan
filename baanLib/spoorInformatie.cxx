#include "spoorInformatie.h"
#include "programDefines.h"
#include "stdio.h"
#include <boost/lexical_cast.hpp>

SpoorInformatie::SpoorInformatie()
{
    m_stationInfo.reset(new xmlpp::DomParser);
    xmlpp::Document *stationDoc = m_stationInfo->get_document();
    /*xmlpp::Element* nodeRoot = */stationDoc->create_root_node("stationInfo");
}

SpoorInformatie::~SpoorInformatie()
{

}

void SpoorInformatie::AddStation(std::string station)
{
    std::string data="name";

   xmlpp::Document *stationDoc = m_stationInfo->get_document();

    //foo is the default namespace prefix.
    xmlpp::Element* nodeRoot = stationDoc->get_root_node();
    // check of die er al is
    std::string xpath="station[@name='"+station+"']";
    xmlpp::NodeSet stat=nodeRoot->find(xpath);
    if (stat.empty())
    {
        xmlpp::Element* nodeChild = nodeRoot->add_child("station");
        nodeChild->set_attribute("name", station);
    }
}


void SpoorInformatie::AddSpoor(std::string station, std::string spoor)
{
    xmlpp::Document *stationDoc = m_stationInfo->get_document();

    xmlpp::Element* nodeRoot = stationDoc->get_root_node();
    
    // check of die er al is
    std::string xpath="station[@name='"+station+"']/spoor[@name='"+spoor+"']";
    xmlpp::NodeSet stat=nodeRoot->find(xpath);
    if (stat.empty())
    {
        std::string xpath="station[@name='"+station+"']";
        xmlpp::NodeSet stat=nodeRoot->find(xpath);
        xmlpp::Element* nodeChild = stat[0]->add_child("spoor");
        nodeChild->set_attribute("name", spoor);
    }
}

void SpoorInformatie::AddBlok(std::string station, std::string spoor, int blok)
{
    xmlpp::Document *stationDoc = m_stationInfo->get_document();

    xmlpp::Element* nodeRoot = stationDoc->get_root_node();
    
    // check of die er al is
    std::string xpath="station[@name='"+station+"']/spoor[@name='"+spoor+"']" +
        "[blok=" + boost::lexical_cast<std::string>(blok) + "]";
    xmlpp::NodeSet stat=nodeRoot->find(xpath);
    if (stat.empty())
    {
        std::string xpath="station[@name='"+station+"']/spoor[@name='"+spoor+"']";
        xmlpp::NodeSet stat=nodeRoot->find(xpath);
        xmlpp::Element* nodeChild = stat[0]->add_child("blok");
        nodeChild->set_child_text(boost::lexical_cast<std::string>(blok));
    }
}

void SpoorInformatie::AddStopBlok(std::string station, std::string spoor, int blok, int richting)
{
    xmlpp::Document *stationDoc = m_stationInfo->get_document();

    xmlpp::Element* nodeRoot = stationDoc->get_root_node();
    
    // check of die er al is
    std::string xpath="station[@name='"+station+"']/spoor[@name='"+spoor+"']" +
        "[blok[@stopBlok='"+boost::lexical_cast<std::string>(richting)+"']=" + boost::lexical_cast<std::string>(blok) + "]";
    xmlpp::NodeSet stat=nodeRoot->find(xpath);
    if (stat.empty())
    {
        std::string xpath="station[@name='"+station+"']/spoor[@name='"+spoor+"']";
        xmlpp::NodeSet stat=nodeRoot->find(xpath);
        xmlpp::Element* newChild;
        xmlpp::Node::NodeList blokList=stat[0]->get_children("blok");
        int length = blokList.size();
        xmlpp::Node * node= blokList.front();
        if ((richting == RICHTING_ACHTERUIT) && (length!=0) )
        {
            newChild = stat[0]->add_child_before(node,"blok");
        }
        else
        {
            newChild = stat[0]->add_child("blok");
        }
        newChild->set_attribute("richting", boost::lexical_cast<std::string>(richting));
        newChild->set_child_text(boost::lexical_cast<std::string>(blok));
    }
}

int SpoorInformatie::GetStopBlok(std::string station, std::string spoor, int richting)
{
    xmlpp::Document *stationDoc = m_stationInfo->get_document();
    xmlpp::Element* nodeRoot = stationDoc->get_root_node();
    std::string stopBlokText;
    std::string xpath="station[@name='"+station+"']/spoor[@name='"+spoor+"']" +
        "/blok";
    xmlpp::NodeSet stat=nodeRoot->find(xpath);
    if (!stat.empty())
    {
        // er is een stopblok
        if (richting == RICHTING_VOORUIT)
            stopBlokText = GetTextFromNodeSet(stat.back());
        else
            stopBlokText = GetTextFromNodeSet(stat.front());
    }
    // tja als er niets is dan toch maar het stopblok 0 teruggeven
    if (stopBlokText.empty())
        return 0;
    else
        return boost::lexical_cast<int>(stopBlokText);
}

int SpoorInformatie::GetTreinBlok(std::string station, std::string spoor, int richting)
{
    xmlpp::Document *stationDoc = m_stationInfo->get_document();
    xmlpp::Element* nodeRoot = stationDoc->get_root_node();
    std::string stopBlokText;
    std::string xpath="station[@name='"+station+"']/spoor[@name='"+spoor+"']" +
        "/blok";
    xmlpp::NodeSet stat=nodeRoot->find(xpath);
    if (!stat.empty())
    {
        if (stat.size() >= 2)
        {
            if (richting == RICHTING_VOORUIT)
                stopBlokText = GetTextFromNodeSet(stat[stat.size()-2]);
            else
                stopBlokText = GetTextFromNodeSet(stat[1]);
        }
        else
        {
            stopBlokText = GetTextFromNodeSet(stat[0]);
        }
    }
    // tja als er niets is dan toch maar het stopblok 0 teruggeven
    if (stopBlokText.empty())
        return 0;
    else
        return boost::lexical_cast<int>(stopBlokText);
}

std::string SpoorInformatie::GetTextFromNodeSet(xmlpp::Node *node)
{
    std::string text;
    xmlpp::Element *eenNode = dynamic_cast<xmlpp::Element *>(node);
    if(eenNode){
        text = eenNode->get_child_text()->get_content();
    }
    return text;
}

bool SpoorInformatie::IsBlokAanwezig(std::string station, std::string spoor, int blok)
{
    xmlpp::Document *stationDoc = m_stationInfo->get_document();

    xmlpp::Element* nodeRoot = stationDoc->get_root_node();
    
    // check of die er al is
    std::string xpath="station[@name='"+station+"']/spoor[@name='"+spoor+"']" +
        "[blok=" + boost::lexical_cast<std::string>(blok) + "]";
    xmlpp::NodeSet stat=nodeRoot->find(xpath);
    return !stat.empty();
}


void SpoorInformatie::SaveData(std::string filename)
{
    std::string file=filename + ".xml";
    xmlpp::Document *stationDoc = m_stationInfo->get_document();
    stationDoc->write_to_file_formatted(file);
}

void SpoorInformatie::LoadData(std::string filename)
{
    std::string file=filename + ".xml";
    try
    {
        m_stationInfo->parse_file(file);
    }
    catch (xmlpp::exception& e)
    {
        m_stationInfo.reset(new xmlpp::DomParser);
        xmlpp::Document *stationDoc = m_stationInfo->get_document();
        /*xmlpp::Element* nodeRoot = */stationDoc->create_root_node("stationInfo");
    }
}
