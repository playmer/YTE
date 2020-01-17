#include "basis_universal/basisu_comp.h"

#include "YTE/StandardLibrary/File.hpp"

#include "YTETools/ImportMathTypeConverters.hpp"
#include "YTETools/YTETexture.hpp"

namespace YTE::Tools 
{
  void ImportTextureToFile(std::string const& aFileToRead, std::string const& aFileToOutput, TextureImportSettings aTextureImporting)
  {
    basisu::basis_compressor_params compressorParameters;
    
    //basisu x.png
    compressorParameters.m_force_alpha = true;
    compressorParameters.m_multithreading = true;
    compressorParameters.m_max_endpoint_clusters = 16128;
    compressorParameters.m_max_selector_clusters = 16128;
    compressorParameters.m_compression_level = aTextureImporting.CompressionLevel;
    compressorParameters.m_quality_level = aTextureImporting.QualityLevel;

    // These should be true if normal map, but currently we just always want them on.
		compressorParameters.m_no_selector_rdo = true;
		compressorParameters.m_no_endpoint_rdo = true;
    
    if (aTextureImporting.NormalMap)
    {
			compressorParameters.m_perceptual = false;
			compressorParameters.m_mip_srgb = false;
    }

    basisu::basis_compressor compressor;

    compressor.init(compressorParameters);

    auto& basisData = compressor.get_output_basis_file();

    FileWriter writer{ aFileToOutput };

    writer.Write(basisData.data(), basisData.size());
  }
}