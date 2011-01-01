/*
*	Copyright (C) 2010 Thorsten Liebig (Thorsten.Liebig@gmx.de)
*
*	This program is free software: you can redistribute it and/or modify
*	it under the terms of the GNU General Public License as published by
*	the Free Software Foundation, either version 3 of the License, or
*	(at your option) any later version.
*
*	This program is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*	GNU General Public License for more details.
*
*	You should have received a copy of the GNU General Public License
*	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef PROCESSFIELDS_H
#define PROCESSFIELDS_H

#include "processing.h"
#include "tools/array_ops.h"

#define __VTK_DATA_TYPE__ "double"

class ProcessFields : public Processing
{
public:
	ProcessFields(Engine_Interface_Base* eng_if);
	virtual ~ProcessFields();

	enum FileType { VTK_FILETYPE, HDF5_FILETYPE};
	enum DumpType { E_FIELD_DUMP, H_FIELD_DUMP};

	virtual void InitProcess();

	virtual void DefineStartStopCoord(double* dstart, double* dstop);

	//! Define a field dump sub sampling rate for a given direction (default: \a dir = -1 means all directions)
	virtual void SetSubSampling(unsigned int subSampleRate, int dir=-1);

	//! Define a field dump optimal resolution for a given direction (default: \a dir = -1 means all directions)
	virtual void SetOptResolution(double optRes, int dir=-1);

	//! Used file pattern e.g. pattern="tmp/efield_" --> "tmp/efield_000045.vtk" for timestep 45 or "tmp/efield_2.40000e9.vtk" for 2.4GHz E-field dump. (VTK FileType only) \sa SetFileType()
	void SetFilePattern(string fp) {m_filename=filePattern=fp;}

	//! Set the filename for a hdf5 data group file (HDF5 FileType only) \sa SetFileType()
	void SetFileName(string fn) {m_filename=fn;}

	//! Define the Dump-Mode
	void SetDumpMode(Engine_Interface_Base::InterpolationType mode);
	//! This methode will dump all fields on a main cell node using 2 E-field and 4 H-fields per direction.
	void SetDumpMode2Node() {SetDumpMode(Engine_Interface_Base::NODE_INTERPOLATE);}
	//! This methode will dump all fields in the center of a main cell (dual-node) using 4 E-field and 2 H-fields per direction.
	void SetDumpMode2Cell() {SetDumpMode(Engine_Interface_Base::CELL_INTERPOLATE);}

	//! Set dump type: 0 for E-fields, 1 for H-fields, 2 for D-fields, 3 for B-fields, 4 for J-fields, etc...
	void SetDumpType(DumpType type) {m_DumpType=type;}

	//! Write a vtk header to an already open file with given mesh-type
	static void WriteVTKHeader(ofstream &file, double const* const* discLines, unsigned int const* numLines, unsigned int precision=12, string header_info = string(), MeshType meshT = CARTESIAN_MESH, double discLines_scaling = 1);
	//! Write a vtk header to an already open file (cartesian grid)
	static void WriteVTKCartesianGridHeader(ofstream &file, double const* const* discLines, unsigned int const* numLines, unsigned int precision=12, string header_info = string(), double discLines_scaling = 1);
	//! Write a vtk header to an already open file (cylindrical grid)
	static void WriteVTKCylindricalGridHeader(ofstream &file, double const* const* discLines, unsigned int const* numLines, unsigned int precision=12, string header_info = string(), double discLines_scaling = 1);
	//! Append a vtk vector array to an already open vtk file, write a header first! \sa WriteVTKHeader()
	static void WriteVTKVectorArray(ofstream &file, string name, FDTD_FLOAT const* const* const* const* array, double const* const* discLines, unsigned int const* numLines, unsigned int precision=12, MeshType meshT = CARTESIAN_MESH);
	//! Append a vtk scalar array to an already open vtk file, write a header first! \sa WriteVTKHeader()
	static void WriteVTKScalarArray(ofstream &file, string name, FDTD_FLOAT const* const* const* array, unsigned int const* numLines, unsigned int precision=12);

	static bool DumpVectorArray2VTK(ofstream &file, string name, FDTD_FLOAT const* const* const * const* array, double const* const* discLines, unsigned int const* numLines, unsigned int precision=12, string header_info = string(), MeshType meshT = CARTESIAN_MESH, double discLines_scaling = 1);
	static bool DumpMultiVectorArray2VTK(ofstream &file, string names[], FDTD_FLOAT const* const* const* const* const* array, unsigned int numFields, double const* const* discLines, unsigned int const* numLines, unsigned int precision=12, string header_info = string(), MeshType meshT = CARTESIAN_MESH, double discLines_scaling = 1);
	static bool DumpScalarArray2VTK(ofstream &file, string name, FDTD_FLOAT const* const* const* array, double const* const* discLines, unsigned int const* numLines, unsigned int precision=12, string header_info = string(), MeshType meshT = CARTESIAN_MESH, double discLines_scaling = 1);
	static bool DumpMultiScalarArray2VTK(ofstream &file, string names[], FDTD_FLOAT const* const* const* const* array, unsigned int numFields, double const* const* discLines, unsigned int const* numLines, unsigned int precision=12, string header_info = string(), MeshType meshT = CARTESIAN_MESH, double discLines_scaling = 1);

	//! Write a mesh information to the given hdf5-group
	static bool WriteMesh2HDF5(string filename, string groupName, unsigned int const* numLines, double const* const* discLines, MeshType meshT = CARTESIAN_MESH, double discLines_scaling = 1);

	//! Dump a time-domain vector dump to an HDF5 file
	static bool DumpVectorArray2HDF5(string filename, string groupName, string name, FDTD_FLOAT const* const* const* const* array, unsigned int const* numLines, float time=0);

	//! Dump a frequency-domain complex-vector dump to an HDF5 file
	static bool DumpVectorArray2HDF5(string filename, string groupName, string name, std::complex<float> const* const* const* const* array, unsigned int const* numLines, float weight, float frequency);

	double CalcTotalEnergy() const;

	void SetFileType(FileType fileType) {m_fileType=fileType;}

	static string GetFieldNameByType(DumpType type);

protected:
	DumpType m_DumpType;
	string filePattern;
	FileType m_fileType;

	enum SampleType {NONE, SUBSAMPLE, OPT_RESOLUTION} m_SampleType;
	virtual void CalcMeshPos();

	//! field dump sub-sampling (if enabled)
	unsigned int subSample[3];

	//! field dump optimal resolution (if enabled)
	double optResolution[3];

	//! dump mesh information
	unsigned int numLines[3];	//number of lines to dump
	unsigned int* posLines[3];	//grid positions to dump
	double* discLines[3];		//mesh disc lines to dump

	//! Calculate and return the defined field. Caller has to cleanup the array.
	FDTD_FLOAT**** CalcField();
};

#endif // PROCESSFIELDS_H