/*
NavPower.h - v2.0.0
A header file for use with NavPower's binary navmesh files.

Licensed under the MIT License
SPDX-License-Identifier: MIT
Copyright (c) 2022+ Anthony Fuller et al.

Permission is hereby  granted, free of charge, to any  person obtaining a copy
of this software and associated  documentation files (the "Software"), to deal
in the Software  without restriction, including without  limitation the rights
to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <algorithm>
#include <stdlib.h>

#include "Glacier/ZMath.hpp"

uint32_t RangeCheck(uint32_t val, uint32_t min, uint32_t max);

inline uint32_t c_byteswap_ulong(uint32_t p_Value) {
#if _MSC_VER
    return _byteswap_ulong(p_Value);
#else
    return ((p_Value >> 24) & 0x000000FF) | ((p_Value >> 8) & 0x0000FF00) | ((p_Value << 8) & 0x00FF0000) | ((p_Value << 24) & 0xFF000000);
#endif
}

namespace NavPower {
    class BBox {
      public:
        SVector3 m_min;
        SVector3 m_max;
    };

    enum Axis { X, Y, Z, UNDEF };

    enum class AreaUsageFlags : uint32_t {
        AREA_FLAT = 1,
        AREA_STEPS = 8,
    };

    enum EdgeType { EDGE_NORMAL, EDGE_PORTAL };

    class Area;

    // The binary formats of NavPower data structures
    namespace Binary {
        class Header {
          public:
            uint32_t m_endianFlag = 0;
            uint32_t m_version = 2;
            uint32_t m_imageSize; // Size of the file excluding this header
            uint32_t m_checksum;  // Checksum of data excluding this header
            uint32_t m_runtimeFlags = 0;
            uint32_t m_constantFlags = 0;
        };

        class SectionHeader {
          public:
            uint32_t m_id = 0x10000;
            uint32_t m_size;            // Size of this section excluding this header
            uint32_t m_pointerSize = 1; // Size of pointers inside the section
        };

        class NavSetHeader {
          public:
            uint32_t m_endianFlag = 0;
            uint32_t m_version = 0x28; // The version of the nav graph, this case it is 40
            uint32_t m_numGraphs = 1;  // Number of NavGraphs in this image
        };

        class NavGraphHeader {
          public:
            uint32_t m_version = 0x30;
            uint32_t m_layer = 0;
            uint32_t m_areaBytes;
            uint32_t m_kdTreeBytes;
            uint32_t m_unknown0 = 0;
            uint32_t m_unknown1 = 0;
            uint32_t m_unknown2 = 56;
            uint32_t m_linkRecordBytes = 0;
            uint32_t m_totalBytes;
            uint32_t m_unknown3 = 0;
            float m_buildScale = 2.0f;
            float m_voxSize = 0.1f;
            float m_radius = 0.2f;
            float m_stepHeight = 0.3f;
            float m_height = 1.8f; // Human Height
            BBox m_bbox;
            Axis m_buildUpAxis = Axis::Z;
            // In NAVPs from 007 First Light the padding isn't just 0x00
            // It is however identical in all files, changing it to all 0x00 makes NPCs disappear completely
            uint8_t m_pad[252] = {0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                                  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                                  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                                  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                                  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                                  0,   0,   0,   0,   0,   0,   0,   0,   0,   191, 20,  214, 126, 45,  220, 142, 102, 131, 239, 87,  73,  97,  255,
                                  105, 143, 97,  205, 209, 30,  157, 156, 22,  114, 114, 230, 29,  240, 132, 79,  74,  119, 2,   215, 232, 57,  44,
                                  83,  203, 201, 18,  30,  51,  116, 158, 12,  244, 213, 212, 159, 212, 164, 89,  126, 53,  207, 50,  34,  244, 204,
                                  207, 211, 144, 45,  72,  211, 143, 117, 230, 217, 29,  42,  229, 192, 247, 43,  120, 129, 135, 68,  14,  95,  80,
                                  0,   212, 97,  141, 190, 123, 5,   21,  7,   59,  51,  130, 31,  24,  112, 146, 218, 100, 84,  206, 177, 133, 62,
                                  105, 21,  248, 70,  106, 4,   150, 115, 14,  217, 22,  47,  103, 104, 212, 247, 74,  74,  208, 87,  104, 118};

            void writeBinary(std::ostream& f);
        };

        class AreaFlags {
          public:
            // flag 1
            uint32_t GetNumEdges() const {
                return m_flags1 & 0x7F;
            }
            void SetNumEdges(uint32_t p_numEdges) {
                m_flags1 = (m_flags1 & ~0x7F) | (p_numEdges & 0x7F);
            }
            uint32_t GetIslandNum() const {
                return (m_flags1 >> 7) & 0x3FFFF;
            }
            void SetIslandNum(uint32_t p_islandNum) {
                m_flags1 = (m_flags1 & ~0x3FFFF) | ((p_islandNum << 7) & 0x3FFFF);
            }

            bool IsImpassable() {
                return (m_flags1 & 0x20000000) != 0;
            }
            bool ApplyObCostWhenFlagsDontMatch() {
                return (m_flags1 & 0x10000000) != 0;
            }

            // flag 2
            uint32_t GetAreaUsageCount() const {
                return m_flags2 & 0x3FF;
            }
            void SetAreaUsageCount(uint32_t p_usageCount) {
                m_flags2 = (m_flags2 & ~0x3FF) | (p_usageCount & 0x3FF);
            }
            uint32_t GetObCostMult() const {
                return (m_flags2 & 0x000F0000) >> 16;
            }
            void SetObCostMult(uint32_t p_obCostMult) {
                m_flags2 = (m_flags2 & ~0x000F0000) | ((RangeCheck(p_obCostMult, 1, 15) << 16) & 0x000F0000);
            }
            uint32_t GetStaticCostMult() const {
                return (m_flags2 & 0x00F00000) >> 20;
            }
            void SetStaticCostMult(uint32_t p_staticCostMult) {
                m_flags2 = (m_flags2 & ~0x00F00000) | ((RangeCheck(p_staticCostMult, 1, 15) << 20) & 0x00F00000);
            }
            uint32_t GetBasisVert() const {
                return (m_flags2 & 0x7F000000) >> 24;
            }
            void SetBasisVert(uint32_t basisVert) {
                m_flags2 = (m_flags2 & ~0x7F000000) | ((basisVert << 24) & 0x7F000000);
            }

            // flag 3
            // there's a few functions here but the data is always zero

            // flag 4
            // there's one function here which returns the flag, always zero

            uint32_t m_flags1;
            uint32_t m_flags2;
            uint32_t m_flags3;
            uint32_t m_flags4;
        };

        class Edge;

        class Area {
          public:
            uint64_t m_pProxy = 0;
            uint64_t m_dynAreaData = 0;
            uint64_t m_pFirstLink = 0;
            uint64_t m_pSearchParent = 0;
            SVector3 m_pos;
            float m_radius;
            uint32_t m_searchCost = 0xFFFFFFFF;
            AreaUsageFlags m_usageFlags;
            AreaFlags m_flags;

            [[nodiscard]] Edge* GetFirstEdge() {
                return reinterpret_cast<Edge*>(reinterpret_cast<uintptr_t>(this) + sizeof(Area));
            }
        };

        class Edge {
          public:
            Area* m_pAdjArea;
            SVector3 m_pos;
            uint32_t m_flags1;
            uint32_t m_flags2;
            char m_pad[4] = "\0\0\0";

            // flags 1
            // Obstacles
            uint32_t GetObID() const {
                return m_flags1 & 0x7FFF;
            }
            void SetObID(uint32_t p_Value) {
                m_flags1 |= p_Value & 0x7FFF;
            }

            // Partition logic
            bool GetPartition() const {
                return (m_flags1 & 0x1000) != 0;
            }
            void SetPartition(bool partition) {
                if (partition) {
                    m_flags1 |= 0x1000;
                }
                else {
                    m_flags1 &= ~0x1000;
                }
            }

            // Normal or Portal
            EdgeType GetType() const {
                return (EdgeType)((m_flags1 & 0x8000) >> 15);
            }
            void SetType(EdgeType p_EdgeType) {
                m_flags1 |= (p_EdgeType) << 15;
            }
        };

        class KDTreeData {
          public:
            BBox m_bbox;
            uint32_t m_size;

            void writeBinary(std::ostream& f);
        };

        class KDLeaf {
          public:
            uint32_t m_data = 0x80000000;

            uint32_t GetPrimOffset() const {
                return m_data & 0x7FFFFFFF;
            }
            void SetPrimOffset(uint32_t primOffset) {
                m_data |= primOffset & 0x7FFFFFFF;
            }
            void SetIsLeaf(int p_isLeaf) {
                if (p_isLeaf) {
                    m_data |= 0x80000000;
                }
                else {
                    m_data &= ~0x80000000;
                }
            }
        };

        class KDNode {
          public:
            uint32_t m_data;
            float m_dLeft;
            float m_dRight;

            bool IsLeaf() {
                return m_data & 0x80000000 ? true : false;
            }
            void SetIsLeaf(int p_isLeaf) {
                m_data = (m_data & ~0x80000000) | (p_isLeaf & 0x80000000);
            }
            Axis GetSplitAxis() {
                return (Axis)((m_data >> 28) & 7);
            }
            void SetSplitAxis(Axis p_axis) {
                m_data = (m_data & ~0x70000000) >> 28 | ((p_axis << 28) & 0x70000000);
            }
            uint32_t GetRightOffset() {
                return m_data & 0xFFFFFFF;
            }
            void SetRightOffset(uint32_t p_rightOffset) {
                m_data = (m_data & ~0xFFFFFFF) | (p_rightOffset & 0xFFFFFFF);
            }
            KDNode* GetLeft() {
                return this + 1;
            }
            KDNode* GetRight() {
                return (KDNode*)((char*)this + GetRightOffset());
            }
        };
    }; // namespace Binary

    // Requires a pointer to the NavGraph, fixes Area pointers in the NavGraph
    // The reason we do this is so it actually points to an area in memory and
    // not a pointer to a location in the file relative to the start of the NavGraph
    void FixAreaPointers(uintptr_t data, size_t areaBytes) {
        uintptr_t navGraphStart = data;
        uintptr_t curIndex = data + sizeof(Binary::NavGraphHeader);
        size_t areaEndPtr = curIndex + areaBytes;

        while (curIndex != areaEndPtr) {
            Binary::Area* curArea = reinterpret_cast<Binary::Area*>(curIndex);
            curIndex += sizeof(Binary::Area);
            for (uint32_t i = 0; i < curArea->m_flags.GetNumEdges(); i++) {
                Binary::Edge* curEdge = (Binary::Edge*)curIndex;
                curIndex += sizeof(Binary::Edge);

                Binary::Area* adjArea = curEdge->m_pAdjArea;
                if (adjArea != NULL) {
                    curEdge->m_pAdjArea = (Binary::Area*)(navGraphStart + (char*)adjArea);
                }
            }
        }
    }

    class Area {
      public:
        Binary::Area* m_area;
        std::vector<Binary::Edge*> m_edges;

        SVector3 CalculateNormal() {
            SVector3 v0 = m_edges.at(0)->m_pos;
            SVector3 v1 = m_edges.at(1)->m_pos;
            SVector3 basis = m_edges.at(2)->m_pos;

            SVector3 vec1 = basis - v0;
            SVector3 vec2 = v1 - v0;
            SVector3 cross = vec1.Cross(vec2);

            return cross.GetUnitVec();
        }

        SVector3 CalculateCentroid() {
            SVector3 normal = CalculateNormal();
            SVector3 v0 = m_edges.at(0)->m_pos;
            SVector3 v1 = m_edges.at(1)->m_pos;

            SVector3 u = (v1 - v0).GetUnitVec();
            SVector3 v = u.Cross(normal).GetUnitVec();

            std::vector<SVector3> mappedPoints;
            for (Binary::Edge* edge : m_edges) {
                SVector3 relativePos = edge->m_pos - v0;
                float uCoord = relativePos.Dot(u);
                float vCoord = relativePos.Dot(v);
                SVector3 uvv = SVector3(uCoord, vCoord, 0.0);
                mappedPoints.push_back(uvv);
            }
            float sum = 0;
            for (int i = 0; i < mappedPoints.size(); i++) {
                int nextI = (i + 1) % mappedPoints.size();
                sum += mappedPoints[i].x * mappedPoints[nextI].y - mappedPoints[nextI].x * mappedPoints[i].y;
            }
            float area = sum / 2;
            if (area < 0) {
                area *= -1;
            }

            float sumX = 0;
            float sumY = 0;
            for (int i = 0; i < mappedPoints.size(); i++) {
                int nextI = (i + 1) % mappedPoints.size();
                float x0 = mappedPoints[i].x;
                float x1 = mappedPoints[nextI].x;
                float y0 = mappedPoints[i].y;
                float y1 = mappedPoints[nextI].y;

                float doubleArea = (x0 * y1) - (x1 * y0);
                sumX += (x0 + x1) * doubleArea;
                sumY += (y0 + y1) * doubleArea;
            }

            float cu = sumX / (6.0 * area);
            float cv = sumY / (6.0 * area);

            SVector3 cucv = SVector3(1, cu, cv);
            SVector3 xuv = SVector3(v0.x, u.x, v.x);
            SVector3 yuv = SVector3(v0.y, u.y, v.y);
            SVector3 zuv = SVector3(v0.z, u.z, v.z);
            float x = xuv.Dot(cucv);
            float y = yuv.Dot(cucv);
            float z = zuv.Dot(cucv);
            return SVector3(x, y, z);
        }
    };

    // Helps with outputting the k-d tree as Bounding Boxes
    struct KDTreeHelper {
        Binary::KDNode* m_node;
        BBox m_bbox;
        uint32_t m_depth;
        uint32_t m_splitAxis;
    };

    class NavGraph {
      public:
        Binary::NavGraphHeader* m_hdr;
        std::vector<Area> m_areas;
        Binary::KDTreeData* m_kdTreeData;
        Binary::KDNode* m_rootKDNode;

        NavGraph() {};
        NavGraph(auto s_NavGraphJson);

        NavGraph(uintptr_t p_data) {
            read(p_data);
        };

        void read(uintptr_t& p_data) {
            uintptr_t s_startPointer = p_data;
            uintptr_t s_endPointer{};

            m_hdr = (Binary::NavGraphHeader*)p_data;
            p_data += sizeof(Binary::NavGraphHeader);

            FixAreaPointers(p_data - sizeof(Binary::NavGraphHeader), m_hdr->m_areaBytes);

            s_endPointer = p_data + m_hdr->m_areaBytes;
            while (p_data < s_endPointer) {
                Area s_area{};
                s_area.m_area = (Binary::Area*)p_data;
                p_data += sizeof(Binary::Area);

                for (uint32_t i = 0; i < s_area.m_area->m_flags.GetNumEdges(); ++i) {
                    s_area.m_edges.push_back((Binary::Edge*)p_data);
                    p_data += sizeof(Binary::Edge);
                }

                m_areas.push_back(s_area);
            }
            m_kdTreeData = (Binary::KDTreeData*)p_data;
            p_data += sizeof(Binary::KDTreeData);

            m_rootKDNode = (Binary::KDNode*)p_data;

            s_endPointer = p_data + m_kdTreeData->m_size;
            while (p_data < s_endPointer) {
                Binary::KDNode* s_KDNode = (Binary::KDNode*)p_data;
                Binary::KDLeaf* s_KDLeaf = (Binary::KDLeaf*)p_data;

                if (s_KDNode->IsLeaf()) {
                    p_data += sizeof(Binary::KDLeaf);
                }
                else {
                    p_data += sizeof(Binary::KDNode);
                }
            }

            if ((p_data - s_startPointer) != m_hdr->m_totalBytes) {
                printf("[WARNING] NavGraph - What we read does not match the total bytes!\n");
            }
        }
    };

    class Section {
      public:
        Binary::SectionHeader* m_hdr;
        Binary::NavSetHeader* m_setHdr;
        std::vector<NavGraph> m_aNavGraphs;

        Section() {};
        Section(uintptr_t& p_data) {
            read(p_data);
        };

        void read(uintptr_t& p_data) {
            m_hdr = (Binary::SectionHeader*)p_data;
            p_data += sizeof(Binary::SectionHeader);

            if (m_hdr->m_size == 0) {
                return;
            }

            // Sectionheader->m_size excludes the header.
            uintptr_t s_startPointer = p_data;
            uintptr_t s_endPointer{};

            m_setHdr = (Binary::NavSetHeader*)p_data;
            p_data += sizeof(Binary::NavSetHeader);

            for (uint32_t i = 0; i < m_setHdr->m_numGraphs; ++i) {
                NavGraph s_Graph;
                s_Graph.read(p_data);
                m_aNavGraphs.push_back(s_Graph);
            }

            if ((p_data - s_startPointer) != m_hdr->m_size) {
                printf("[WARNING] Section - What we read does not match the section size!\n");
            }
        }
    };

    class NavMesh {
      public:
        Binary::Header* m_hdr;
        std::vector<Section> m_aSections;

        NavMesh() {};
        NavMesh(const char* p_NavGraphJsonPath);
        NavMesh(uintptr_t p_data, uint32_t p_filesize) {
            read(p_data, p_filesize);
        };

        void read(uintptr_t p_data, uint32_t p_filesize) {
            uintptr_t s_startPointer = p_data;

            m_hdr = (Binary::Header*)p_data;
            p_data += sizeof(Binary::Header);

            // Read Sections
            while ((p_data - s_startPointer) < p_filesize) {
                Section s_Section;
                s_Section.read(p_data);
                m_aSections.push_back(s_Section);
            }
        }
    };
} // namespace NavPower
