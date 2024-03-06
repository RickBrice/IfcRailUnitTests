// IfcRailUnitTests.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

// Disable warnings coming from IfcOpenShell
#pragma warning(disable:4018 4267 4250 4984 4985)

#include <iostream>
#include <filesystem>
#include <fstream>

#include <ifcparse/IfcHierarchyHelper.h>
#include <ifcparse/Ifc4x3_add2.h>
#include <ifcgeom/abstract_mapping.h>

#define Schema Ifc4x3_add2

void cant_alignment(const std::string& ifc_filename, const std::string& results_filename)
{
   IfcParse::IfcFile file(ifc_filename);
   auto curves = file.instances_by_type<Schema::IfcSegmentedReferenceCurve>();
   auto curve = (*(curves->begin()))->as<Schema::IfcSegmentedReferenceCurve>();

   //ifcopenshell::geometry::Settings settings;
   //auto mapping = ifcopenshell::geometry::impl::mapping_implementations().construct(&file, settings);

   ////settings.get<ifcopenshell::geometry::settings::PiecewiseStepType>().value = ifcopenshell::geometry::settings::PiecewiseStepMethod::MAXSTEPSIZE;
   ////settings.get<ifcopenshell::geometry::settings::PiecewiseStepParam>().value = 1.0;

   //auto pwf = ifcopenshell::geometry::taxonomy::cast<ifcopenshell::geometry::taxonomy::piecewise_function>(mapping->map(curve));

   //std::ofstream of(results_filename);
   //assert(of.is_open());
   //of << "Vertical Alignment Curve (each curve divided in 100 parts)" << std::endl;
   //of << "[Horizontal Distance]" << '\t' << "[X-Coordinate]" << '\t' << "[Y-Coordinate]" << std::endl;
   //of << std::setprecision(std::numeric_limits<double>::digits10);


   //for (int i = 0; i <= 100; i++)
   //{
   //   double s = (double)i;
   //   auto m = ifcopenshell::geometry::taxonomy::make<ifcopenshell::geometry::taxonomy::matrix4>(pwf->evaluate(s));

   //   m->components().col(3).head(3) /= mapping->get_length_unit();
   //   Eigen::Matrix4d values = m->components();

   //   double x = values(0, 3); // row, col
   //   double y = values(2, 3);

   //   of << s << '\t' << x << '\t' << y << std::endl;
   //}
}

void vertical_alignment(const std::string& ifc_filename, const std::string& results_filename)
{
   IfcParse::IfcFile file(ifc_filename);
   auto curves = file.instances_by_type<Schema::IfcGradientCurve>();
   auto curve = (*(curves->begin()))->as<Schema::IfcGradientCurve>();

   ifcopenshell::geometry::Settings settings;
   auto mapping = ifcopenshell::geometry::impl::mapping_implementations().construct(&file, settings);

   //settings.get<ifcopenshell::geometry::settings::PiecewiseStepType>().value = ifcopenshell::geometry::settings::PiecewiseStepMethod::MAXSTEPSIZE;
   //settings.get<ifcopenshell::geometry::settings::PiecewiseStepParam>().value = 1.0;

   auto pwf = ifcopenshell::geometry::taxonomy::cast<ifcopenshell::geometry::taxonomy::piecewise_function>(mapping->map(curve));

   std::ofstream of(results_filename);
   assert(of.is_open());
   of << "Vertical Alignment Curve (each curve divided in 100 parts)" << std::endl;
   of << "[Horizontal Distance]" << '\t' << "[X-Coordinate]" << '\t' << "[Y-Coordinate]" << std::endl;
   of << std::setprecision(std::numeric_limits<double>::digits10);


   for (int i = 0; i <= 100; i++)
   {
      double s = (double)i;
      auto m = ifcopenshell::geometry::taxonomy::make<ifcopenshell::geometry::taxonomy::matrix4>(pwf->evaluate(s));

      m->components().col(3).head(3) /= mapping->get_length_unit();
      Eigen::Matrix4d values = m->components();

      double x = values(0, 3); // row, col
      double y = values(2, 3);

      of << s << '\t' << x << '\t' << y << std::endl;
   }
}

void horizontal_alignment(const std::string& ifc_filename, const std::string& results_filename)
{
   IfcParse::IfcFile file(ifc_filename);
   auto curves = file.instances_by_type<Schema::IfcCompositeCurve>();
   auto curve = (*(curves->begin()))->as<Schema::IfcCompositeCurve>();

   ifcopenshell::geometry::Settings settings;
   auto mapping = ifcopenshell::geometry::impl::mapping_implementations().construct(&file, settings);

   //settings.get<ifcopenshell::geometry::settings::PiecewiseStepType>().value = ifcopenshell::geometry::settings::PiecewiseStepMethod::MAXSTEPSIZE;
   //settings.get<ifcopenshell::geometry::settings::PiecewiseStepParam>().value = 1.0;

   auto pwf = ifcopenshell::geometry::taxonomy::cast<ifcopenshell::geometry::taxonomy::piecewise_function>(mapping->map(curve));

   std::ofstream of(results_filename);
   assert(of.is_open());
   of << "Horizontal Alignment Curve (each curve divided in 100 parts)" << std::endl;
   of << "[Curve Length]" << '\t' << "[X-Coordinate]" << '\t' << "[Y-Coordinate]" << std::endl;

   of << std::setprecision(std::numeric_limits<double>::digits10);

   for (int i = 0; i <= 100; i++)
   {
      double s = (double)i;
      auto m = ifcopenshell::geometry::taxonomy::make<ifcopenshell::geometry::taxonomy::matrix4>(pwf->evaluate(s));

      m->components().col(3).head(3) /= mapping->get_length_unit();
      Eigen::Matrix4d values = m->components();

      double x = values(0, 3); // row, col
      double y = values(1, 3);

      of << s << '\t' << x << '\t' << y << std::endl;
   }
}

int main()
{
   // iterate over everything in the directory (not recursive)
   for (const auto& file : std::filesystem::directory_iterator("F:/IFC-Rail-Unit-Test-Reference-Code/alignment_testset/IFC-WithGeneratedGeometry"))
   {
      const auto& file_path = std::filesystem::path(file);
      if (file_path.extension() != ".ifc") continue; // if it's not an IFC file, go to next item

      auto filename = file_path.filename().string();

      // The following code is a total hack since I'm not very good at string processing
      // The object is to figure out the alignment type (HorizontalAlignment, VerticalAlignment, CantAlignment)
      // and the curve type (Bloss, Cosine, etc) that is encoded in the IFC file name
      
      
      // Assume file name begins with GENERATED__ followed by the alignment type
      auto iter = filename.begin() + 11; // 11 = GENERATED__
      auto found = filename.find_first_of("_", 12);
      auto iter2 = filename.begin() + found;

      auto alignment_type = filename.substr(std::distance(filename.begin(),iter), std::distance(iter, iter2));

      // not dealing with INDEXEDPOLYCURVE yet, so just go to the next item
      if (alignment_type == "INDEXEDPOLYCURVE") continue;

      // Curve type should follow alignment name
      auto found2 = filename.find_first_of("_", found + 1);
      auto iter3 = filename.begin() + found2-1;

      auto curve_type = filename.substr(found+1, std::distance(iter2, iter3));

      // the curve parameters, which are needed to define the output file name, are next
      iter3 += 2;
      auto suffix = filename.substr(std::distance(filename.begin(), iter3),filename.length()-4);
      suffix.replace(suffix.find("ifc"), 3, "txt"); // output is TXT

      // Output directory
      auto results_file = std::string("F:/IFC-Rail-Unit-Test-Reference-Code/alignment_testset/ToolboxProcess-IfcOpenShell/")// root directory for my unit test results
         + alignment_type + "/" + curve_type + "/";  // subdirectories
      if (!std::filesystem::exists(results_file))
      {
         // it doesn't exist so create it... creates the full path as needed
         std::filesystem::create_directories(results_file);
      }
      
      // finish creating the output file name
      results_file += curve_type + "_" + suffix; // filename

      if (alignment_type == "HorizontalAlignment")
         horizontal_alignment(file_path.string(), results_file);
      else if(alignment_type == "VerticalAlignment")
         vertical_alignment(file_path.string(), results_file);
      else if (alignment_type == "CantAlignment")
      {
         // not ready for cant yet
         //cant_alignment(file_path.string(), results_file);
      }
      else
      {
      }
   }
}
